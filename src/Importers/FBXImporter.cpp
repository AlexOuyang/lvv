//
//  FBXImporter.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 6/1/14.
//
//

#include <glm/gtx/euler_angles.hpp>

#include "FBXImporter.h"

#include "Core/Scene.h"
#include "Shapes/Mesh.h"
#include "Shapes/AnimatedMesh.h"
#include "Core/TransformedPrimitive.h"
#include "Core/GeometricPrimitive.h"
#include "Materials/Matte.h"
#include "Lights/PointLight.h"
#include "Lights/DirectionalLight.h"
#include "Lights/AreaLight.h"
#include "Utilities/ImageLoading.h"
#include "Shapes/ShapesUtilities.h"

std::shared_ptr<FBXImporter> FBXImporter::Load(const rapidjson::Value&) {
    // No configuration to load, just create an importer
    return std::make_shared<FBXImporter>();
}

FBXImporter::FBXImporter() : SceneImporter(),
_fbxManager(nullptr), _importedMaterials() {
    _fbxManager = FbxManager::Create();
}

FBXImporter::~FBXImporter() {
    _fbxManager->Destroy();
}

bool FBXImporter::import(Scene &scene) {
    FbxImporter* fbxImporter = FbxImporter::Create(_fbxManager, "");
    
    if (!fbxImporter->Initialize(_filename.c_str())) {
        std::cerr << "FbxImporter error: " << fbxImporter->GetStatus().GetErrorString() << std::endl;
        return false;
    }
    
    FbxScene* fbxScene = FbxScene::Create(_fbxManager, "ImportedScene");
    
    if (!fbxImporter->Import(fbxScene)) {
        std::cerr << "FbxImporter error: " << fbxImporter->GetStatus().GetErrorString() << std::endl;
        return false;
    }
    
    FbxNode* rootNode = fbxScene->GetRootNode();
    if (!rootNode) {
        return false;
    }
    
    importNode(rootNode, scene);
    
    return true;
}

void FBXImporter::importNode(FbxNode *fbxNode, Scene &scene, const Transform& parentTransform) {
    // Import node transform
    Transform transform = importNodeTransform(fbxNode);
    transform = parentTransform(transform);
    
    // Import node attributes
    for (uint_t i = 0; i < fbxNode->GetNodeAttributeCount(); ++i) {
        importNodeAttribute(fbxNode, fbxNode->GetNodeAttributeByIndex(i), scene, transform);
    }
    
    for (uint_t i = 0; i < fbxNode->GetChildCount(); ++i) {
        importNode(fbxNode->GetChild(i), scene, transform);
    }
}

void FBXImporter::importNodeAttribute(FbxNode* fbxNode, FbxNodeAttribute* fbxAttribute,
                                      Scene& scene,
                                      const Transform& transform) {
    FbxNodeAttribute::EType type = fbxAttribute->GetAttributeType();
    
    if (type == FbxNodeAttribute::eMesh) {
        FbxMesh* fbxMesh = dynamic_cast<FbxMesh*>(fbxAttribute);
        importMesh(fbxNode, fbxMesh, scene, transform);
    }
    
    if (type == FbxNodeAttribute::eLight) {
        const FbxLight* fbxLight = dynamic_cast<const FbxLight*>(fbxAttribute);
        
        Light* light = nullptr;
        
        if (fbxLight->LightType.Get() == FbxLight::ePoint) {
            PointLight* point = new PointLight();
            
            vec3 position = vec3(0.f, 0.f, 0.f);
            point->setPosition(transform(position));
            point->setSpectrum(Spectrum(importVec3(fbxLight->Color.Get())));
            point->setIntensity(fbxLight->Intensity.Get() * 0.01f);
            point->setNoDecay(fbxLight->DecayType.Get() == FbxLight::eNone);
            
            light = point;
        } else if (fbxLight->LightType.Get() == FbxLight::eDirectional) {
            DirectionalLight* directional = new DirectionalLight();
            
            vec3 direction = vec3(0.f, 0.f, -1.f);
            directional->setDirection(normalize(transform.applyToVector(direction)));
            directional->setSpectrum(Spectrum(importVec3(fbxLight->Color.Get())));
            directional->setIntensity(fbxLight->Intensity.Get() * 0.01f);
            
            light = directional;
        } else {
            std::cout << "FBXImporter error: unsuported light type for \"" << fbxNode->GetName()
            << "\"" << std::endl;
        }
        
        if (light) {
            light->setName(fbxNode->GetName());
            if (applyLightOverrides(scene, light)) {
                scene << light;
            }
        }
        
    }
    
    if (type == FbxNodeAttribute::eCamera) {
        FbxCamera* fbxCamera = dynamic_cast<FbxCamera*>(fbxAttribute);
        
        std::shared_ptr<PerspectiveCamera> camera = std::make_shared<PerspectiveCamera>();
        camera->setName(fbxNode->GetName());
        
        float aspect = fbxCamera->FilmAspectRatio.Get();
        camera->setAspect(aspect);
        if (fbxCamera->ApertureMode.Get() == FbxCamera::eFocalLength) {
            camera->setVFov(fbxCamera->ComputeFieldOfView(fbxCamera->FocalLength.Get())/aspect);
        } else {
            camera->setVFov(glm::radians(fbxCamera->FieldOfView.Get())/aspect);
        }
        
        camera->lookAt(importVec3(fbxCamera->Position.Get()), importVec3(fbxCamera->InterestPosition.Get()),
                       importVec3(fbxCamera->UpVector.Get()));
        
        if (isNodeAnimated(fbxNode)) {
            scene.registerAnimationEvaluator(std::make_shared<CameraAnimationEvaluator>(fbxCamera,
                                                                                        camera,
                                                                                        shared_from_this()));
        }
        
        scene << camera;
    }
}

void FBXImporter::importMesh(FbxNode* fbxNode, FbxMesh* fbxMesh, Scene &scene,
                             const Transform &transform) {
    if (!fbxMesh->IsTriangleMesh()) {
        return;
    }
    
    std::string name = fbxNode->GetName();
    
    // Load mesh data
    uint_t indicesCount, verticesCount;
    uint_t* indices;
    uint_t* materialIndices = nullptr;
    Vertex* vertices;
    bool hasUVs = false;
    
    FbxTime time = FBXSDK_TIME_INFINITE;
    if (!_loadMeshData(fbxMesh, &verticesCount, &vertices, &indicesCount,
                       &indices, &materialIndices, &hasUVs, time)) {
        return;
    }
    
    // Create mesh
    std::shared_ptr<MeshBase> mesh;
    std::shared_ptr<AnimatedMesh> animated;
    
    if (fbxMesh->GetDeformerCount() == 0) {
        // Create static mesh
        mesh = std::make_shared<Mesh>();
    } else {
        // Create animated mesh with same vertices
        animated = std::make_shared<AnimatedMesh>();
        
        // Create end vertices
        Vertex* endVertices = new Vertex[verticesCount];
        for (uint_t i = 0; i < verticesCount; ++i) {
            endVertices[i] = vertices[i];
        }
        
        animated->setEndVertices(endVertices);
        
        mesh = animated;
    }
    
    // Set mesh base data
    mesh->setVertices(verticesCount, vertices);
    mesh->setIndices(indicesCount, indices);
    
    if (materialIndices) {
        mesh->setMaterialIndices(materialIndices);
    }
    
    // Generate mesh tangents
    mesh->generateTangents(hasUVs);
    
    // Load mesh materials
    std::vector<ImportedMaterial> materials;
    _importNodeMaterials(materials, fbxNode, scene);
    
    // If material indices are providen, assign materials to mesh
    if (materialIndices) {
        std::vector<std::shared_ptr<Material>> meshMaterials;
        meshMaterials.reserve(materials.size());
        for (ImportedMaterial& material : materials) {
            meshMaterials.push_back(material.second);
        }
        mesh->setMaterials(meshMaterials);
    }
    
    if (materials.size() == 1 && materials[0].first.alphaTexture) {
        mesh->setAlphaTexture(materials[0].first.alphaTexture);
    }
    
    std::shared_ptr<GeometricPrimitive> primitive = std::make_shared<GeometricPrimitive>(mesh,
                                                                                         materials[0].second);
    
    // Apply overrides
    bool addToScene = applyPrimitiveOverrides(scene, name, transform, *primitive, &materials[0].first,
                                              mesh.get());
    
    if (addToScene) {
        // Create mesh acceleration structure
        std::shared_ptr<Aggregate> aggregate = createMeshAccelerationStructure();
        *aggregate << primitive;
        
        // Build acceleration structure
        aggregate->preprocess();
        
        // Create transformed primitive
        std::shared_ptr<TransformedPrimitive> transformedPrimitive =
        //std::make_shared<TransformedPrimitive>(aggregate, Transform());
        std::make_shared<TransformedPrimitive>(aggregate, transform);
        
        transformedPrimitive->setName(name);
        
        // Create animation evaluators
        if (isNodeAnimated(fbxNode)) {
            scene.registerAnimationEvaluator(std::make_shared<MeshAnimationEvaluator>(fbxNode,
                                                                                      transformedPrimitive,
                                                                                      shared_from_this()));
        }
        if (animated) {
            scene
            .registerAnimationEvaluator(std::make_shared<SkinnedMeshAnimationEvaluator>(fbxMesh,
                                                                                        animated,
                                                                                        aggregate,
                                                                                        shared_from_this()));
        }
        
        // Add primitive to scene
        scene << transformedPrimitive;
    }
}

void FBXImporter::_importNodeMaterials(std::vector<ImportedMaterial>& materials,
                                       const FbxNode *fbxNode, Scene &scene) {
    if (fbxNode->GetMaterialCount() == 0) {
        std::cerr << "FbxImport warning: no material for node \"" << fbxNode->GetName() << "\"" << std::endl;
        materials.push_back(std::make_pair(ImportedMaterialAttributes(), std::make_shared<Matte>()));
    }
    
    for (uint_t i = 0; i < fbxNode->GetMaterialCount(); ++i) {
        std::shared_ptr<Material> material;
        
        FbxSurfaceMaterial* fbxMaterial = fbxNode->GetMaterial(i);
        
        // Check if material already exists
        auto importedIt = _importedMaterials.find(fbxMaterial);
        if (importedIt != _importedMaterials.end()) {
            materials.push_back(importedIt->second);
            continue;
        }
        
        FbxSurfaceLambert* fbxLambert = dynamic_cast<FbxSurfaceLambert*>(fbxMaterial);
        FbxSurfacePhong* fbxPhong = dynamic_cast<FbxSurfacePhong*>(fbxMaterial);

        ImportedMaterialAttributes attrs;
        attrs.name = fbxMaterial->GetName();
        
        if (fbxLambert) {
            attrs.shadingMode = ImportedMaterialAttributes::Lambert;
            attrs.diffuseColor = importVec3(fbxLambert->Diffuse.Get());
            attrs.diffuseTexture = importTexture(fbxLambert->Diffuse.GetSrcObject<FbxTexture>(0));
            attrs.diffuseIntensity = fbxLambert->DiffuseFactor.Get();
            attrs.diffuseIntensityTexture =
            importTexture(fbxLambert->DiffuseFactor.GetSrcObject<FbxTexture>(0), true);
            attrs.alphaTexture = importTexture(fbxLambert->TransparentColor.GetSrcObject<FbxTexture>(0), true);
            attrs.normalMap = importTexture(fbxLambert->NormalMap.GetSrcObject<FbxTexture>(0));
        }
        if (fbxPhong) {
            attrs.shadingMode = ImportedMaterialAttributes::Phong;
            attrs.specularColor = importVec3(fbxPhong->Specular.Get());
            attrs.specularTexture = importTexture(fbxPhong->Specular.GetSrcObject<FbxTexture>(0));
            attrs.specularIntensityTexture =
            importTexture(fbxPhong->ReflectionFactor.GetSrcObject<FbxTexture>(0), true);
        }
        
        ImportedMaterial imported = std::make_pair(attrs, addImportedMaterial(attrs, scene));
        
        _importedMaterials[fbxMaterial] = imported;
        
        materials.push_back(imported);
    }
}

bool FBXImporter::_loadMeshData(FbxMesh* fbxMesh, uint_t *verticesCount, Vertex **vertices,
                                uint_t *indicesCount, uint_t **indices, uint_t** materialIndices,
                                bool* hasUVs, const FbxTime& time) const {
    bool reIndexVertices = false;
    
    // Load indices
    *indicesCount = fbxMesh->GetPolygonVertexCount();
    *indices = new uint_t[*indicesCount];
    int* polygonIndices = fbxMesh->GetPolygonVertices();
    for (uint_t i = 0; i < *indicesCount; ++i) {
        (*indices)[i] = polygonIndices[i];
    }
    
    // Get normal and uv elements
    const FbxGeometryElementNormal* normalElement = fbxMesh->GetElementNormal();
    const FbxGeometryElementUV* uvElement = fbxMesh->GetElementUV();
    
    if (uvElement) {
        *hasUVs = true;
    }
    
    if (!normalElement) {
        std::cerr << "FBXImporter error: no normals provided for mesh \"" << fbxMesh->GetNode()->GetName()
        << "\", mesh ignored" << std::endl;
        return false;
    }
    if (normalElement->GetMappingMode() != FbxGeometryElement::eByPolygonVertex
        && normalElement->GetMappingMode() != FbxGeometryElement::eByControlPoint) {
        std::cerr << "FBXImporter error: unsuported normal mapping mode for mesh \"" << fbxMesh->GetNode()->GetName()
        << "\", mesh ignored" << std::endl;
        return false;
    }
    
    // Get mesh uv set name
    std::string uvName;
    FbxStringList uvNames;
    fbxMesh->GetUVSetNames(uvNames);
    if (uvNames.GetCount() > 0) {
        uvName = uvNames[0];
    } else {
        *hasUVs = false;
    }

    if (normalElement->GetMappingMode() == FbxGeometryElement::eByPolygonVertex ||
        (uvElement
         && uvElement->GetMappingMode() == FbxGeometryElement::eByPolygonVertex))
        {
        reIndexVertices = true;
    }
    
    // Get control points array
    uint_t controlPointsCount = fbxMesh->GetControlPointsCount();
    vec3* controlPoints = new vec3[controlPointsCount];
    for (uint_t i = 0; i < controlPointsCount; ++i) {
        controlPoints[i] = importVec3(fbxMesh->GetControlPointAt(i));
    }
    
    // Get normals (per control point) array
    vec3* normals = new vec3[controlPointsCount];
    bool usePerVertexNormals = true;
    if (normalElement->GetMappingMode() == FbxGeometryElement::eByPolygonVertex) {
        // Average normals per vertices
        for (uint_t i = 0; i < controlPointsCount; ++i) {
            normals[i] = vec3(0.f);
        }
        
        for (uint_t i = 0; i < fbxMesh->GetPolygonCount(); ++i) {
            for (uint_t j = 0; j < 3; ++j) {
                uint_t controlPoint = fbxMesh->GetPolygonVertex(i, j);
                FbxVector4 normal;
                if (fbxMesh->GetPolygonVertexNormal(i, j, normal)) {
                    normals[controlPoint] += importVec3(normal);
                }
            }
        }
        
        // Normalize
        for (uint_t i = 0; i < controlPointsCount; ++i) {
            normals[i] = normalize(normals[i]);
        }
    } else if (normalElement->GetMappingMode() == FbxGeometryElement::eByControlPoint) {
        for (uint_t i = 0; i < controlPointsCount; ++i) {
            int normalIndex = 0;
            if (normalElement->GetReferenceMode() == FbxGeometryElement::eDirect) {
                normalIndex = i;
            } else if (normalElement->GetReferenceMode() == FbxGeometryElement::eIndexToDirect) {
                normalIndex = normalElement->GetIndexArray().GetAt(i);
            }
            
            normals[i] = importVec3(normalElement->GetDirectArray().GetAt(normalIndex));
        }
    }
    
    if (!_applyMeshSkinning(fbxMesh, &controlPointsCount, &controlPoints, &normals, time)) {
        usePerVertexNormals = false;
    }
    
    if (reIndexVertices) {
        // Create vertices for each indices
        *verticesCount = *indicesCount;
        *vertices = new Vertex[*verticesCount];
        uint_t vertexIndex = 0;
        
        // Create vertices for each polygon vertex (each index)
        for (uint_t i = 0; i < fbxMesh->GetPolygonCount(); ++i) {
            for (uint_t j = 0; j < 3; ++j) {
                uint_t controlPoint = fbxMesh->GetPolygonVertex(i, j);
                (*vertices)[vertexIndex].position = controlPoints[controlPoint];
                if (usePerVertexNormals) {
                    (*vertices)[vertexIndex].normal = normals[controlPoint];
                } else {
                    FbxVector4 normal;
                    if (fbxMesh->GetPolygonVertexNormal(i, j, normal)) {
                        (*vertices)[vertexIndex].normal = importVec3(normal);
                    }
                }
                FbxVector2 uv;
                bool unMapped = true;
                if (!uvName.empty() && fbxMesh->GetPolygonVertexUV(i, j, uvName.c_str(), uv, unMapped)) {
                    if (!unMapped) {
                        (*vertices)[vertexIndex].texCoord = importVec2(uv);
                    } else {
                        *hasUVs = false;
                    }
                }
                (*indices)[vertexIndex] = vertexIndex;
                ++vertexIndex;
            }
        }
    } else {
        // Load vertices
        *verticesCount = fbxMesh->GetControlPointsCount();
        *vertices = new Vertex[*verticesCount];
        for (uint_t i = 0; i < *verticesCount; ++i) {
            (*vertices)[i].position = importVec3(fbxMesh->GetControlPointAt(i));
        }
        
        // Load normals
        if (usePerVertexNormals) {
            for (uint_t i = 0; i < *verticesCount; ++i) {
                (*vertices)[i].normal = normals[i];
            }
        } else if (normalElement->GetMappingMode() == FbxGeometryElement::eByControlPoint) {
            for (uint_t i = 0; i < *verticesCount; ++i) {
                int normalIndex = 0;
                if (normalElement->GetReferenceMode() == FbxGeometryElement::eDirect) {
                    normalIndex = i;
                } else if (normalElement->GetReferenceMode() == FbxGeometryElement::eIndexToDirect) {
                    normalIndex = normalElement->GetIndexArray().GetAt(i);
                }
                
                (*vertices)[i].normal = importVec3(normalElement->GetDirectArray().GetAt(normalIndex));
            }
        }
        // Load texture coords
        if (uvElement) {
            if (uvElement->GetMappingMode() == FbxGeometryElement::eByControlPoint) {
                for (uint_t i = 0; i < *verticesCount; ++i) {
                    int uvIndex = 0;
                    if (uvElement->GetReferenceMode() == FbxGeometryElement::eDirect) {
                        uvIndex = i;
                    } else if (uvElement->GetReferenceMode() == FbxGeometryElement::eIndexToDirect) {
                        uvIndex = uvElement->GetIndexArray().GetAt(i);
                    }
                    
                    (*vertices)[i].texCoord = importVec2(uvElement->GetDirectArray().GetAt(uvIndex));
                }
            }
        }
    }
    
    // Cleanup
    delete[] controlPoints;
    delete[] normals;
    
    // Get material indices
    FbxLayerElementArrayTemplate<int>* fbxMaterialIndices = nullptr;
    if (fbxMesh->GetMaterialIndices(&fbxMaterialIndices)
        && fbxMaterialIndices->GetCount() == (*indicesCount / 3)) {
        *materialIndices = new uint_t[(*indicesCount / 3)];
        for (uint_t i = 0; i < (*indicesCount / 3); ++i) {
            (*materialIndices)[i] = fbxMaterialIndices->GetAt(i);
        }
    }
    
    return true;
}

bool FBXImporter::_applyMeshSkinning(FbxMesh* fbxMesh, uint_t* controlPointsCount,
                                     vec3** controlPoints, vec3** normals,
                                     const FbxTime& time) const {
    // Deform control points from skin
    
    /*
     Skinned mesh:
     - get skin deformer
     - for each cluster
     -  get bone transform
     -  get bone binding matrix: inverse(transformLinkMatrix) * transformMatrix
     -  apply transform to each control point into a new array
     */
    
    if (fbxMesh->GetDeformerCount() < 1) {
        return false;
    }
    
    // Init skinned control points array
    vec3* skinnedControlPoints = new vec3[*controlPointsCount];
    for (uint_t i = 0; i < *controlPointsCount; ++i) {
        skinnedControlPoints[i] = vec3(0.f);
    }
    vec3* skinnedNormals = new vec3[*controlPointsCount];
    for (uint_t i = 0; i < *controlPointsCount; ++i) {
        skinnedNormals[i] = vec3(0.f);
    }
    
    const FbxSkin* fbxSkin = dynamic_cast<FbxSkin*>(fbxMesh->GetDeformer(0));
    if (fbxSkin) {
        for (uint_t i = 0; i < fbxSkin->GetClusterCount(); ++i) {
            const FbxCluster* fbxCluster = fbxSkin->GetCluster(i);
            
            if (fbxCluster->GetLinkMode() == FbxCluster::eAdditive) {
                continue;
            }
            
            // Get cluster link node (bone)
            FbxNode* bone = const_cast<FbxNode*>(fbxCluster->GetLink());
            
            // Extract bone matrix at current time
            FbxAMatrix& fbxBoneTransform = bone->EvaluateGlobalTransform(time);
            mat4x4 boneTransform = importMatrix(fbxBoneTransform);
            
            // Change bone transform from world-space to object space
            Transform objectTransfrom = importNodeTransform(fbxMesh->GetNode());
            boneTransform = glm::inverse(objectTransfrom.getMatrix()) * boneTransform;
            
            // Get bind matrix
            FbxAMatrix fbxTransformMatrix, fbxTransformLinkMatrix;
            fbxCluster->GetTransformMatrix(fbxTransformMatrix);
            fbxCluster->GetTransformLinkMatrix(fbxTransformLinkMatrix);
            mat4x4 transformMatrix = importMatrix(fbxTransformMatrix);
            mat4x4 transformLinkMatrix = importMatrix(fbxTransformLinkMatrix);
            
            mat4x4 bindMatrix = glm::inverse(transformLinkMatrix) * transformMatrix;
            
            uint_t controlPointsIndicesCount = fbxCluster->GetControlPointIndicesCount();
            int* controlPointsIndices = fbxCluster->GetControlPointIndices();
            double* controlPointsWeight = fbxCluster->GetControlPointWeights();
            
            for (uint_t i = 0; i < controlPointsIndicesCount; ++i) {
                uint_t pointIndex = controlPointsIndices[i];
                float pointWeight = controlPointsWeight[i];
                mat4x4 skinMatrix = pointWeight * boneTransform * bindMatrix;
                mat4x4 normalSkinMatrix = glm::inverse(glm::transpose(skinMatrix));
                skinnedControlPoints[pointIndex] += vec3(skinMatrix * vec4((*controlPoints)[pointIndex], 1.f));
                skinnedNormals[pointIndex] += vec3(normalSkinMatrix * vec4((*normals)[pointIndex], 0.f));
            }
        }
    }
    
    // Copy skinned points into control points array
    for (uint_t i = 0; i < *controlPointsCount; ++i) {
        (*controlPoints)[i] = skinnedControlPoints[i];
    }
    for (uint_t i = 0; i < *controlPointsCount; ++i) {
        (*normals)[i] = normalize(skinnedNormals[i]);
    }
    
    // Clean
    delete[] skinnedControlPoints;
    delete[] skinnedNormals;
    
    return true;
}

vec3 FBXImporter::importVec3(const FbxVector4& v) {
    return vec3(v[0], v[1], v[2]);
}

vec2 FBXImporter::importVec2(const FbxVector2& v) {
    return vec2(v[0], v[1]);
}

mat4x4 FBXImporter::importMatrix(const FbxAMatrix& m) {
    mat4x4 mat;
    
    for (uint_t i = 0; i < 4; ++i) {
        for (uint_t j = 0; j < 4; ++j) {
            mat[i][j] = m.Get(i, j);
        }
    }
    
    return mat;
}

Transform FBXImporter::importNodeTransform(const FbxNode* node) {
    Transform t;
    
    vec3 translation = importVec3(node->LclTranslation.Get());
    vec3 rotation = importVec3(node->LclRotation.Get());
    vec3 scale = importVec3(node->LclScaling.Get());
    
    EFbxRotationOrder rotationOrder = node->RotationOrder.Get();
    mat4x4 rotationMatrix;
    
    if (rotationOrder == eEulerXYZ) {
        rotationMatrix = (glm::eulerAngleZ(glm::radians(rotation[2])))
                          * glm::eulerAngleY(glm::radians(rotation[1]))
                          * glm::eulerAngleX(glm::radians(rotation[0]));
    } else if (rotationOrder == eEulerXZY) {
        rotationMatrix = (glm::eulerAngleY(glm::radians(rotation[1]))
                          * glm::eulerAngleZ(glm::radians(rotation[2]))
                          * glm::eulerAngleX(glm::radians(rotation[0])));
    } else if (rotationOrder == eEulerYZX) {
        rotationMatrix = (glm::eulerAngleX(glm::radians(rotation[0]))
                          * glm::eulerAngleZ(glm::radians(rotation[2]))
                          * glm::eulerAngleY(glm::radians(rotation[1])));
    } else if (rotationOrder == eEulerYXZ) {
        rotationMatrix = (glm::eulerAngleZ(glm::radians(rotation[2]))
                          * glm::eulerAngleX(glm::radians(rotation[0]))
                          * glm::eulerAngleY(glm::radians(rotation[1])));
    } else if (rotationOrder == eEulerZXY) {
        rotationMatrix = (glm::eulerAngleY(glm::radians(rotation[1]))
                          * glm::eulerAngleX(glm::radians(rotation[0]))
                          * glm::eulerAngleZ(glm::radians(rotation[2])));
    } else if (rotationOrder == eEulerZYX) {
        rotationMatrix = (glm::eulerAngleX(glm::radians(rotation[0]))
                          * glm::eulerAngleY(glm::radians(rotation[1]))
                          * glm::eulerAngleZ(glm::radians(rotation[2])));
    }
    
    vec3 rotationPivot = importVec3(node->RotationPivot.Get());
    vec3 scalePivot = importVec3(node->ScalingPivot.Get());
    
    t.translate(translation);
    t.translate(rotationPivot);
    t.applyMatrix(rotationMatrix);
    t.translate(-rotationPivot);
    t.translate(scalePivot);
    t.scale(scale);
    t.translate(-scalePivot);
    
    return t;
}

std::shared_ptr<Texture> FBXImporter::importTexture(FbxTexture* fbxTexture,
                                                    bool isFloat) {
    FbxFileTexture* fbxFileTexture = dynamic_cast<FbxFileTexture*>(fbxTexture);
    
    if (!fbxFileTexture) {
        return std::shared_ptr<Texture>();
    }
    
    std::string path = fbxFileTexture->GetFileName();
    std::shared_ptr<Texture> texture;
    if (isFloat) {
        texture = ImageLoading::LoadFloatImage(path);
    } else {
        texture = ImageLoading::LoadImage(path);
    }
    if (!texture) {
        std::cerr << "FbxImporter error: couldn't load texture \"" << path << "\"" << std::endl;
    }
    return texture;
}

bool FBXImporter::isNodeAnimated(FbxNode* fbxNode) {
    FbxTimeSpan span;
    return fbxNode->GetAnimationInterval(span);
}

FBXImporter::CameraAnimationEvaluator::CameraAnimationEvaluator(FbxCamera* fbxCamera,
                                                                const std::shared_ptr<PerspectiveCamera>&
                                                                camera,
                                                                const std::shared_ptr<FBXImporter>& importer)
: fbxCamera(fbxCamera), camera(camera), importer(importer) {
}

FBXImporter::CameraAnimationEvaluator::~CameraAnimationEvaluator() {
    
}

void FBXImporter::CameraAnimationEvaluator::evaluate(float tstart, float tend) {
    AnimatedTransform anim;
    
    FbxTime time;
    time.SetFramePrecise(tstart, FbxTime::eFrames24);
    vec3 target = importVec3(fbxCamera->EvaluateLookAtPosition(time));
    Transform t1;
    t1.lookAt(importVec3(fbxCamera->EvaluatePosition(time)), target);
    
    time.SetFramePrecise(tend, FbxTime::eFrames24);
    Transform t2;
    t2.lookAt(importVec3(fbxCamera->EvaluatePosition(time)),
              importVec3(fbxCamera->EvaluateLookAtPosition(time)));
    
    camera->setFocusPoint(target);
    
    anim.setTransforms(t1, t2);
    camera->setAnimatedTransform(anim);
}

FBXImporter::MeshAnimationEvaluator::MeshAnimationEvaluator(FbxNode* fbxNode,
                                                            const std::shared_ptr<TransformedPrimitive>& tp,
                                                            const std::shared_ptr<FBXImporter>& importer)
: fbxNode(fbxNode), transformedPrimitive(tp), importer(importer) {
    
}

FBXImporter::MeshAnimationEvaluator::~MeshAnimationEvaluator() {
    
}

void FBXImporter::MeshAnimationEvaluator::evaluate(float tstart, float tend) {
    AnimatedTransform anim;
    Transform t1, t2;

    FbxTime time;
    time.SetFramePrecise(tstart, FbxTime::eFrames24);
    t1.setMatrix(FBXImporter::importMatrix(fbxNode->EvaluateGlobalTransform(time)));
    time.SetFramePrecise(tend, FbxTime::eFrames24);
    t2.setMatrix(FBXImporter::importMatrix(fbxNode->EvaluateGlobalTransform(time)));
    
    anim.setTransforms(t1, t2);
    transformedPrimitive->setAnimatedTransform(anim);
}

FBXImporter::SkinnedMeshAnimationEvaluator::
SkinnedMeshAnimationEvaluator(FbxMesh* fbxMesh,
                              const std::shared_ptr<AnimatedMesh>& mesh,
                              const std::shared_ptr<Aggregate>& aggregate,
                              const std::shared_ptr<FBXImporter>& importer)
: fbxMesh(fbxMesh), mesh(mesh), aggregate(aggregate), importer(importer) {
    
}

FBXImporter::SkinnedMeshAnimationEvaluator::~SkinnedMeshAnimationEvaluator() {
    
}

void FBXImporter::SkinnedMeshAnimationEvaluator::evaluate(float tstart, float tend) {
    // Re-load mesh data at start and end times
    // Load mesh data
    uint_t indicesCount, verticesCount;
    uint_t* indices;
    uint_t* materialIndices = nullptr;
    Vertex* vertices;
    bool hasUVs = false;
    
    FbxTime time;
    time.SetFramePrecise(tstart, FbxTime::eFrames24);
    if (!importer->_loadMeshData(fbxMesh, &verticesCount, &vertices, &indicesCount,
                                 &indices, &materialIndices, &hasUVs, time)) {
        std::cerr << "SkinnedMeshAnimation error: Unable to evaluate animation" << std::endl;
        return;
    }
    
    // Re-set mesh data
    mesh->setVertices(verticesCount, vertices);
    mesh->setIndices(indicesCount, indices);
    mesh->setMaterialIndices(materialIndices);
    
    // Get end-vertices
    time.SetFramePrecise(tend, FbxTime::eFrames24);
    if (!importer->_loadMeshData(fbxMesh, &verticesCount, &vertices, &indicesCount,
                                 &indices, &materialIndices, &hasUVs, time)) {
        std::cerr << "SkinnedMeshAnimation error: Unable to evaluate animation" << std::endl;
        return;
    }
    mesh->setEndVertices(vertices);
    
    // Delete unused indices
    delete[] indices;
    if (materialIndices) {
        delete[] materialIndices;
    }
    
    // Re-generate mesh tangents
    mesh->generateTangents(hasUVs);
    
    // Re-build aggregate
    aggregate->rebuild();
}