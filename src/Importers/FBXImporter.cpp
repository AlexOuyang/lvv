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
#include "Core/TransformedPrimitive.h"
#include "Core/GeometricPrimitive.h"
#include "Materials/Matte.h"
#include "Lights/PointLight.h"
#include "Lights/DirectionalLight.h"
#include "Lights/AreaLight.h"
#include "Cameras/PerspectiveCamera.h"
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

void FBXImporter::importNode(const FbxNode *fbxNode, Scene &scene, const Transform& parentTransform) {
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

void FBXImporter::importNodeAttribute(const FbxNode* fbxNode, const FbxNodeAttribute* fbxAttribute,
                                      Scene& scene,
                                      const Transform& transform) {
    FbxNodeAttribute::EType type = fbxAttribute->GetAttributeType();
    
    if (type == FbxNodeAttribute::eMesh) {
        const FbxMesh* fbxMesh = dynamic_cast<const FbxMesh*>(fbxAttribute);
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
        const FbxCamera* fbxCamera = dynamic_cast<const FbxCamera*>(fbxAttribute);
        
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
        
        scene << camera;
    }
}

void FBXImporter::importMesh(const FbxNode* fbxNode, const FbxMesh* fbxMesh, Scene &scene,
                             const Transform &transform) {
    if (!fbxMesh->IsTriangleMesh()) {
        return;
    }
    
    std::string name = fbxNode->GetName();
    
    // Load mesh data
    uint_t indicesCount, verticesCount;
    uint_t* indices;
    Vertex* vertices;
    
    _loadMeshData(fbxMesh, &verticesCount, &vertices, &indicesCount, &indices);
    
    // Create mesh
    std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
    
    mesh->setVertices(verticesCount, vertices);
    mesh->setIndices(indicesCount, indices);
    
    // Generate mesh tangents
    mesh->generateTangents();
    
    // Load mesh material
    ImportedMaterial material = _importNodeMaterial(fbxNode, scene);
    
    if (material.first.alphaTexture) {
        mesh->setAlphaTexture(material.first.alphaTexture);
    }
    
    std::shared_ptr<GeometricPrimitive> primitive = std::make_shared<GeometricPrimitive>(mesh,
                                                                                         material.second);
    
    // Apply overrides
    bool addToScene = applyPrimitiveOverrides(scene, name, transform, *primitive, &material.first,
                                              mesh.get());
    
    if (addToScene) {
        // Create mesh acceleration structure
        std::shared_ptr<Aggregate> aggregate = createMeshAccelerationStructure();
        *aggregate << primitive;
        
        // Build acceleration structure
        aggregate->preprocess();
        
        // Create transformed primitive
        std::shared_ptr<TransformedPrimitive> transformedPrimitive =
        std::make_shared<TransformedPrimitive>(aggregate, transform);
        
        // Add primitive to scene
        scene << transformedPrimitive;
    }
}

FBXImporter::ImportedMaterial FBXImporter::_importNodeMaterial(const FbxNode *fbxNode, Scene &scene) {
    std::shared_ptr<Material> material;
    
    if (fbxNode->GetMaterialCount() == 0) {
        std::cerr << "FbxImport warning: no material for node \"" << fbxNode->GetName() << "\"" << std::endl;
        return std::make_pair(ImportedMaterialAttributes(), std::make_shared<Matte>());
    }
    
    FbxSurfaceMaterial* fbxMaterial = fbxNode->GetMaterial(0);
    
    // Check if material already exists
    auto importedIt = _importedMaterials.find(fbxMaterial);
    if (importedIt != _importedMaterials.end()) {
        return importedIt->second;
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
        attrs.diffuseIntensityTexture = importTexture(fbxLambert->DiffuseFactor.GetSrcObject<FbxTexture>(0),
                                                      true);
        attrs.alphaTexture = importTexture(fbxLambert->TransparentColor.GetSrcObject<FbxTexture>(0), true);
        attrs.normalMap = importTexture(fbxLambert->NormalMap.GetSrcObject<FbxTexture>(0));
    }
    if (fbxPhong) {
        attrs.shadingMode = ImportedMaterialAttributes::Phong;
        attrs.specularColor = importVec3(fbxPhong->Specular.Get());
        attrs.specularTexture = importTexture(fbxPhong->Specular.GetSrcObject<FbxTexture>(0));
        attrs.specularIntensityTexture = importTexture(fbxPhong->ReflectionFactor.GetSrcObject<FbxTexture>(0),
                                                       true);
    }
    
    ImportedMaterial imported = std::make_pair(attrs, addImportedMaterial(attrs, scene));
    
    _importedMaterials[fbxMaterial] = imported;
    
    return imported;
}

void FBXImporter::_loadMeshData(const FbxMesh* fbxMesh, uint_t *verticesCount, Vertex **vertices,
                                uint_t *indicesCount, uint_t **indices) const {
    bool reIndexVertices = false;
    
    // Load indices
    *indicesCount = fbxMesh->GetPolygonVertexCount();
    *indices = new uint_t[*indicesCount];
    int* polygonIndices = fbxMesh->GetPolygonVertices();
    for (uint_t i = 0; i < *indicesCount; ++i) {
        (*indices)[i] = polygonIndices[i];
    }
    
    // Check normals mapping mode to decide wether we should re-index vertices or not
    const FbxGeometryElementNormal* normalElement = fbxMesh->GetElementNormal();
    const FbxGeometryElementUV* uvElement = fbxMesh->GetElementUV();
    
    // Get mesh uv set name
    std::string uvName;
    FbxStringList uvNames;
    fbxMesh->GetUVSetNames(uvNames);
    if (uvNames.GetCount() > 0) {
        uvName = uvNames[0];
    }

    if ((normalElement
         && normalElement->GetMappingMode() == FbxGeometryElement::eByPolygonVertex) ||
        (uvElement
         && uvElement->GetMappingMode() == FbxGeometryElement::eByPolygonVertex))
        {
        reIndexVertices = true;
    }
    
    // Load vertices
    
    if (reIndexVertices) {
        // Create vertices for each indices
        *verticesCount = *indicesCount;
        *vertices = new Vertex[*verticesCount];
        uint_t vertexIndex = 0;
        // Create vertices for each polygon vertex (each index)
        for (uint_t i = 0; i < fbxMesh->GetPolygonCount(); ++i) {
            for (uint_t j = 0; j < 3; ++j) {
                uint_t controlPoint = fbxMesh->GetPolygonVertex(i, j);
                (*vertices)[vertexIndex].position = importVec3(fbxMesh->GetControlPointAt(controlPoint));
                FbxVector4 normal;
                if (fbxMesh->GetPolygonVertexNormal(i, j, normal)) {
                    (*vertices)[vertexIndex].normal = importVec3(normal);
                }
                FbxVector2 uv;
                bool unMapped = true;
                if (!uvName.empty() && fbxMesh->GetPolygonVertexUV(i, j, uvName.c_str(), uv, unMapped)
                    && !unMapped) {
                    (*vertices)[vertexIndex].texCoord = importVec2(uv);
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
        if (normalElement) {
            if (normalElement->GetMappingMode() == FbxGeometryElement::eByControlPoint) {
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

}

vec3 FBXImporter::importVec3(const FbxVector4& v) {
    return vec3(v[0], v[1], v[2]);
}

vec2 FBXImporter::importVec2(const FbxVector2& v) {
    return vec2(v[0], v[1]);
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