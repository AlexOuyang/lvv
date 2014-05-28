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
    
    _trianglesCount = 0;
    importNode(assimpScene, assimpScene->mRootNode, scene);
    
    std::cout << "AssimImporter: loaded " << _trianglesCount << " triangles" << std::endl;
    
    return true;
}

void AssimpImporter::importMaterials(const aiScene* assimpScene, Scene& scene) {
    _importedMaterials.clear();
    _importedMaterials.reserve(assimpScene->mNumMaterials);
    for (uint i = 0; i < assimpScene->mNumMaterials; ++i) {
        aiMaterial* assimpMaterial = assimpScene->mMaterials[i];
        
        // Read material properties as assimp values
        aiString assimpName;
        aiString assimpTexturePath;
        aiColor3D assimpColor;
        
        assimpMaterial->Get(AI_MATKEY_NAME, assimpName);
        assimpMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, assimpColor);
        assimpMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &assimpTexturePath);
        
        // Convert them to renderer values
        ImportedMaterialAttributes attrs;
        attrs.name = assimpName.C_Str();
        attrs.diffuseColor = vec3(assimpColor.r, assimpColor.g, assimpColor.b);
        attrs.diffuseTexturePath = assimpTexturePath.C_Str();
        
        _importedMaterials.push_back(addImportedMaterial(attrs, scene));
    }
}

void AssimpImporter::importNode(const aiScene* assimpScene, aiNode* assimpNode,
                                Scene& scene, const mat4x4& parentMatrix) {
    // Get node name
    std::string name = assimpNode->mName.C_Str();
    std::string nameLowercase = name;
    std::transform(name.begin(), name.end(), nameLowercase.begin(), ::tolower);
    
    // Create transform
    mat4x4 transformationMatrix = importMatrix(assimpNode->mTransformation);
    transformationMatrix = parentMatrix * transformationMatrix;
    Transform transform(transformationMatrix);
    
    /*
    // Import camera and lights
    if (camera && nameLowercase.find("camera") != std::string::npos) {
        std::shared_ptr<PerspectiveCamera> perspectiveCamera = std::make_shared<PerspectiveCamera>();
        
        perspectiveCamera->setTransform(Transform(matrix));
        aiCamera* assimpCamera = assimpScene->mCameras[0];
        perspectiveCamera->setVFov(glm::degrees(assimpCamera->mHorizontalFOV)
                                   / assimpCamera->mAspect);
        *camera = perspectiveCamera;
    } else if (_lightsCallback && nameLowercase.find("light") != std::string::npos) {
        for (uint32_t i = 0; i < assimpScene->mNumLights; ++i) {
            aiLight* light = assimpScene->mLights[i];
            if (std::string(light->mName.C_Str()) == name) {
                LightAttributes attrs;
                
                if (light->mType == aiLightSource_DIRECTIONAL) {
                    attrs.type = DirectionalLight;
                } else if (light->mType == aiLightSource_POINT) {
                    attrs.type = PointLight;
                }
                
                attrs.position = importAssimpVec3(light->mPosition);
                attrs.position = vec3(matrix * vec4(attrs.position, 1.0f));
                attrs.color = vec3(light->mColorDiffuse.r,
                                   light->mColorDiffuse.g,
                                   light->mColorDiffuse.b);
                attrs.direction = vec3(light->mDirection.x,
                                       light->mDirection.y,
                                       light->mDirection.z);
                attrs.direction = vec3(matrix * vec4(attrs.direction, 0.0f));
                attrs.intensity = glm::length(attrs.color);
                attrs.color = glm::normalize(attrs.color);
                
                _lightsCallback(attrs);
            }
        }
    }*/
    
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
        uint_t* indices = new uint_t[trianglesCount*3];
        uint_t faceId = 0;
        for (uint_t j = 0; j < assimpMesh->mNumFaces; ++j) {
            if (assimpMesh->mFaces[j].mNumIndices == 3) {
                indices[faceId*3+0] = assimpMesh->mFaces[j].mIndices[0];
                indices[faceId*3+1] = assimpMesh->mFaces[j].mIndices[1];
                indices[faceId*3+2] = assimpMesh->mFaces[j].mIndices[2];
                ++faceId;
            }
        }
        
        // Create mesh
        std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
        
        mesh->setVertices(verticesCount, vertices);
        mesh->setIndices(trianglesCount, indices);
        
        // Create geometric primitive
        std::shared_ptr<Material> material;
        if (assimpMesh->mMaterialIndex < _importedMaterials.size()) {
            material = _importedMaterials[assimpMesh->mMaterialIndex];
        } else {
            std::cerr << "Assimp Importer error: invalid material index";
            return;
        }
        std::shared_ptr<GeometricPrimitive> primitive = std::make_shared<GeometricPrimitive>(mesh, material);
        
        // Apply overrides
        applyPrimitiveOverrides(scene, name, transform, *primitive, mesh.get());
        
        // Create mesh acceleration structure
        std::shared_ptr<Aggregate> aggregate = createMeshAccelerationStructure();
        *aggregate << primitive;
        
        // Build acceleration structure
        aggregate->preprocess();
        
        // Create transformed primitive
        std::shared_ptr<TransformedPrimitive> transformedPrimitive =
        std::make_shared<TransformedPrimitive>(aggregate, transform);
        transformedPrimitive->setName(name);
        
        // Add primitive to scene
        scene << transformedPrimitive;
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