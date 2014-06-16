//
//  AssimpImporter.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/5/14.
//
//

#include "AssimpImporter.h"

std::shared_ptr<AssimpImporter> AssimpImporter::Load(const rapidjson::Value&) {
    // No configuration to load, just create an importer
    return std::make_shared<AssimpImporter>();
}

AssimpImporter::AssimpImporter() : _importer(), _trianglesCount(0), _importedMaterials() {
    
}

AssimpImporter::~AssimpImporter() {
    
}

bool AssimpImporter::import(Scene& scene) {
    // Read the scene from the file
    const aiScene* assimpScene = _importer.ReadFile(_filename, 0
                                                    | aiProcess_Triangulate
                                                    | aiProcess_GenSmoothNormals
                                                    | aiProcess_FindInvalidData);
    
    // If the import failed, report it
    if(!assimpScene) {
        std::cerr << "AssimpImporter Error: " << _importer.GetErrorString() << std::endl;
        return false;
    }
    
    // Import materials
    importMaterials(assimpScene, scene);
    
    // Import scene nodes
    _trianglesCount = 0;
    importNode(assimpScene, assimpScene->mRootNode, scene);
    
    std::cout << "AssimImporter: loaded " << _trianglesCount << " triangles" << std::endl;
    
    return true;
}

bool AssimpImporter::importCamera(const aiScene *assimpScene, aiNode* assimpNode,
                                  const Transform& transform, Scene &scene) {
    // Search for a camera matching the given node name
    for (uint i = 0; i < assimpScene->mNumCameras; ++i) {
        const aiCamera* assimpCamera = assimpScene->mCameras[i];
        const std::string name = assimpCamera->mName.C_Str();
        
        if (name == std::string(assimpNode->mName.C_Str())) {
            std::shared_ptr<PerspectiveCamera> camera = std::make_shared<PerspectiveCamera>();
            camera->setName(name);
            
            // Set camera transform
            camera->setTransform(transform);
            
            // Load camera fov and aspect
            camera->setVFov(glm::degrees(assimpCamera->mHorizontalFOV) / assimpCamera->mAspect);
            camera->setAspect(assimpCamera->mAspect);
            
            // Add camera to the scene
            scene << camera;
            return true;
        }
    }
    return false;
}

bool AssimpImporter::importLight(const aiScene *assimpScene, aiNode* assimpNode,
                                 const Transform& transform, Scene &scene) {
    // Search for a light matching the given node name
    for (uint i = 0; i < assimpScene->mNumLights; ++i) {
        const aiLight* assimpLight = assimpScene->mLights[i];
        const std::string name = assimpLight->mName.C_Str();
        
        if (name == std::string(assimpNode->mName.C_Str())) {
            Light* light = nullptr;
            
            if (assimpLight->mType == aiLightSource_DIRECTIONAL) {
                DirectionalLight* directional =  new DirectionalLight();
                
                directional->setDirection(transform.applyToVector(importVec3(assimpLight->mDirection)));
                directional->setSpectrum(Spectrum(importColor(assimpLight->mColorDiffuse)));
                light = directional;
            } else if (assimpLight->mType == aiLightSource_POINT) {
                PointLight* point = new PointLight();

                point->setPosition(transform(importVec3(assimpLight->mPosition)));
                point->setSpectrum(Spectrum(importColor(assimpLight->mColorDiffuse)));
                light = point;
            }
            
            if (!light) {
                continue;
            }
            light->setName(name);
            
            // Apply light overrides
            bool addToScene = applyLightOverrides(scene, light);
            
            if (addToScene) {
                // Add light to the scene
                scene << light;
            }
            return true;
        }
    }
    return false;
}

void AssimpImporter::importMaterials(const aiScene* assimpScene, Scene& scene) {
    _importedMaterials.clear();
    _importedMaterials.reserve(assimpScene->mNumMaterials);
    for (uint i = 0; i < assimpScene->mNumMaterials; ++i) {
        aiMaterial* assimpMaterial = assimpScene->mMaterials[i];
        
        // Read material properties as assimp values
        aiString assimpName;
        aiColor3D assimpColor;
        aiString assimpDiffuseTexture;
        aiString assimpAlphaTexture;
        
        assimpMaterial->Get(AI_MATKEY_NAME, assimpName);
        assimpMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, assimpColor);
        assimpMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &assimpDiffuseTexture);
        assimpMaterial->GetTexture(aiTextureType_OPACITY, 0, &assimpAlphaTexture);
        
        // Convert them to renderer values
        ImportedMaterialAttributes attrs;
        attrs.name = assimpName.C_Str();
        attrs.shadingMode = ImportedMaterialAttributes::Lambert;
        attrs.diffuseColor = importColor(assimpColor);
        
        // Load textures
        std::string path = assimpDiffuseTexture.C_Str();
        if (!path.empty()) {
            std::shared_ptr<Texture> t = ImageLoading::LoadImage(path);
            if (!t) {
                std::cerr << "AssimpImporter Error: couldn't load texture " << path << std::endl;
            } else {
                attrs.diffuseTexture = t;
            }
        }
        path = assimpAlphaTexture.C_Str();
        if (!path.empty()) {
            std::shared_ptr<Texture> t = ImageLoading::LoadFloatImage(path);
            if (!t) {
                std::cerr << "AssimpImporter Error: couldn't load texture " << path << std::endl;
            } else {
                attrs.alphaTexture = t;
            }
        }
        
        _importedMaterials.push_back(std::make_pair(attrs, addImportedMaterial(attrs, scene)));
    }
}

void AssimpImporter::importNode(const aiScene* assimpScene, aiNode* assimpNode,
                                Scene& scene, const mat4x4& parentMatrix) {
    // Get node name
    std::string name = assimpNode->mName.C_Str();
    
    // Create transform
    mat4x4 transformationMatrix = parentMatrix * importMatrix(assimpNode->mTransformation);
    Transform transform(transformationMatrix);
    
    // Load light
    importLight(assimpScene, assimpNode, transform, scene);
    
    // Load camera
    importCamera(assimpScene, assimpNode, transform, scene);
    
    // Load meshes
    for (uint i = 0; i < assimpNode->mNumMeshes; ++i) {
        aiMesh* assimpMesh = assimpScene->mMeshes[assimpNode->mMeshes[i]];
        
        if (!(assimpMesh->mPrimitiveTypes & aiPrimitiveType_TRIANGLE) ||
            assimpMesh->mNumVertices <= 0) {
            continue ;
        }
        
        // Load vertices
        int verticesCount = assimpMesh->mNumVertices;
        Vertex* vertices = new Vertex[verticesCount];
        for (uint j = 0; j < assimpMesh->mNumVertices; ++j) {
            vertices[j].position = importVec3(assimpMesh->mVertices[j]);
            if (assimpMesh->mNormals) {
                vertices[j].normal = importVec3(assimpMesh->mNormals[j]);
            }
            if (assimpMesh->mTextureCoords[0]) {
                vertices[j].texCoord = vec2(assimpMesh->mTextureCoords[0][j].x,
                                            assimpMesh->mTextureCoords[0][j].y);
            }
        }
        
        // Load triangles
        int trianglesCount = 0;
        for (uint j = 0; j < assimpMesh->mNumFaces; ++j) {
            if (assimpMesh->mFaces[j].mNumIndices == 3) {
                trianglesCount += 1;
            }
        }
        _trianglesCount += trianglesCount;
        uint_t indicesCount = trianglesCount*3;
        uint_t* indices = new uint_t[indicesCount];
        uint_t faceId = 0;
        for (uint_t j = 0; j < assimpMesh->mNumFaces; ++j) {
            if (assimpMesh->mFaces[j].mNumIndices == 3) {
                indices[faceId*3+0] = assimpMesh->mFaces[j].mIndices[0];
                indices[faceId*3+1] = assimpMesh->mFaces[j].mIndices[1];
                indices[faceId*3+2] = assimpMesh->mFaces[j].mIndices[2];
                ++faceId;
            }
        }
        
        // Get mesh material
        ImportedMaterial material;
        if (assimpMesh->mMaterialIndex < _importedMaterials.size()) {
            material = _importedMaterials[assimpMesh->mMaterialIndex];
        } else {
            std::cerr << "Assimp Importer error: invalid material index";
            return;
        }
        
        // Create mesh
        std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
        
        mesh->setVertices(verticesCount, vertices);
        mesh->setIndices(indicesCount, indices);
        
        // Generate mesh tangents
        mesh->generateTangents();
        
        // Create geometric primitive
        if (material.first.alphaTexture) {
            mesh->setAlphaTexture(material.first.alphaTexture);
        }
        
        std::shared_ptr<GeometricPrimitive> primitive = std::make_shared<GeometricPrimitive>(mesh,
                                                                                             material.second);
        
        // Create mesh acceleration structure
        std::shared_ptr<Aggregate> aggregate = createMeshAccelerationStructure();
        
        // Create transformed primitive
        std::shared_ptr<TransformedPrimitive> transformedPrimitive =
        std::make_shared<TransformedPrimitive>(aggregate, transform);
        transformedPrimitive->setName(name);
        
        // Apply overrides
        bool addToScene = applyPrimitiveOverrides(scene, name, transformedPrimitive, *primitive,
                                                  &material.first, mesh.get());
        
        if (addToScene) {
            *aggregate << primitive;
            
            // Build acceleration structure
            aggregate->preprocess();
            
            // Add primitive to scene
            scene << transformedPrimitive;
        }
    }
    
    // Load child nodes recursively
    for (uint i = 0; i < assimpNode->mNumChildren; ++i) {
        importNode(assimpScene, assimpNode->mChildren[i], scene, transformationMatrix);
    }
}

mat4x4 AssimpImporter::importMatrix(const aiMatrix4x4& m) {
    mat4x4 matrix = mat4x4(
                           vec4(m.a1, m.a2, m.a3, m.a4),
                           vec4(m.b1, m.b2, m.b3, m.b4),
                           vec4(m.c1, m.c2, m.c3, m.c4),
                           vec4(m.d1, m.d2, m.d3, m.d4)
                           );
    matrix = glm::transpose(matrix);
    return matrix;
}

vec3 AssimpImporter::importVec3(const aiVector3D& v) {
    return vec3(v.x, v.y, v.z);
}

vec3 AssimpImporter::importColor(const aiColor3D& c) {
    return vec3(c.r, c.g, c.b);
}