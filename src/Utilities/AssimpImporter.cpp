//
//  AssimpImporter.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/5/14.
//
//

#include <iostream>
#include <sstream>

#include "AssimpImporter.h"

#include "Core/TransformedPrimitive.h"
#include "Shapes/Vertex.h"
#include "Shapes/Mesh.h"
#include "Materials/Matte.h"
#include "Materials/Glass.h"
#include "Materials/Glossy.h"
#include "Materials/Metal.h"
#include "Accelerators/BVHAccelerator.h"
#include "Cameras/PerspectiveCamera.h"

AssimpImporter::AssimpImporter() :
_importer(), _materials(), _trianglesCount(),
_materialCallback(nullptr), _defaultMaterial(),
_primitivesCallback(nullptr), _lightsCallback(nullptr) {
    
}

AssimpImporter::~AssimpImporter() {
    
}

void AssimpImporter::setDefaultMaterial(Material *material) {
    _defaultMaterial = material;
}

void AssimpImporter::setMaterialCallback(MaterialCallback callback) {
    _materialCallback = callback;
}

void AssimpImporter::setPrimitivesCallback(PrimitiveCallback callback) {
    _primitivesCallback = callback;
}

void AssimpImporter::setLightsCallback(LightCallback callback) {
    _lightsCallback = callback;
}

bool AssimpImporter::importModel(Aggregate* aggregate, const std::string& filename,
                                 Camera** camera) {
    // Read the scene from the file
    const aiScene* assimpScene = _importer.ReadFile(filename, 0
                                                    | aiProcess_Triangulate
                                                    | aiProcess_GenSmoothNormals
                                                    | aiProcess_FindInvalidData);
    
    // If the import failed, report it
    if(!assimpScene) {
        std::cerr << "AssimImporter Error: " << _importer.GetErrorString() << std::endl;
        return false;
    }
    
    // Import materials
    if (!importAssimpMaterials(assimpScene)) {
        return false;
    }
    
    _trianglesCount = 0;
    bool status = importAssimpNode(aggregate, assimpScene, assimpScene->mRootNode, camera);
    
    std::cout << "AssimImporter: loaded " << _trianglesCount << " triangles" << std::endl;
    
    return status;
}

bool AssimpImporter::importAssimpMaterials(const aiScene* assimpScene) {
    _materials.clear();
    for (uint i = 0; i < assimpScene->mNumMaterials; ++i) {
        aiMaterial* assimpMaterial = assimpScene->mMaterials[i];
        
        aiString nameStr;
        aiString texturePath;
        aiColor3D color;
        aiColor3D transparency;
        int shadingModel = aiShadingMode_Gouraud;
        
        assimpMaterial->Get(AI_MATKEY_NAME, nameStr);
        assimpMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, color);
        assimpMaterial->Get(AI_MATKEY_COLOR_TRANSPARENT, transparency);
        assimpMaterial->Get(AI_MATKEY_SHADING_MODEL, shadingModel);
        assimpMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath);
        
        MaterialAttributes attrs;
        attrs.name = nameStr.C_Str();
        attrs.texturePath = texturePath.C_Str();
        attrs.color = vec3(color.r, color.g, color.b);
        attrs.transparency = vec3(transparency.r, transparency.g, transparency.b);
        switch (shadingModel) {
            case aiShadingMode_Phong:
            case aiShadingMode_Blinn:
            case aiShadingMode_CookTorrance:
                attrs.shadingMode = Phong;
                break;
                
            default:
                attrs.shadingMode = Lambert;
                break;
        }
        
        Material* material = _defaultMaterial;
        
        if (_materialCallback) {
            material = _materialCallback(attrs);
        }
        
        if (!material || !_materialCallback) {
            Matte* matte;
            if (!attrs.texturePath.empty()) {
                Texture* t = ImageLoading::LoadImage(attrs.texturePath);
                if (!t) {
                    std::cerr << "AssimImporter Error: couldn't load texture " << attrs.texturePath << std::endl;
                    matte = new Matte();
                    matte->setColor(attrs.color);
                } else {
                    matte = new Matte(t);
                }
            } else {
                matte = new Matte();
                matte->setColor(attrs.color);
            }
            material = matte;
        }
        
        _materials.push_back(material);
    }
    return true;
}

bool AssimpImporter::importAssimpNode(Aggregate* aggregate, const aiScene* assimpScene,
                                      aiNode* assimpNode, Camera** camera,
                                      const mat4x4& parentMatrix) {
    
    std::string name = assimpNode->mName.C_Str();
    std::string nameLowercase = name;
    std::transform(name.begin(), name.end(), nameLowercase.begin(), ::tolower);

    aiMatrix4x4 t = assimpNode->mTransformation;
    
    mat4x4 matrix = mat4x4(
                           vec4(t.a1, t.a2, t.a3, t.a4),
                           vec4(t.b1, t.b2, t.b3, t.b4),
                           vec4(t.c1, t.c2, t.c3, t.c4),
                           vec4(t.d1, t.d2, t.d3, t.d4)
    );
    matrix = glm::transpose(matrix);
    
    // Multiply matrix by parent matrix
    matrix = parentMatrix * matrix;

    // Import camera and lights
    if (camera && nameLowercase.find("camera") != std::string::npos) {
        PerspectiveCamera* perspectiveCamera = new PerspectiveCamera();
        
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
    }
    
    // Load meshes
    for (uint i = 0; i < assimpNode->mNumMeshes; ++i) {
        aiMesh* assimpMesh = assimpScene->mMeshes[assimpNode->mMeshes[i]];
        
        if (!(assimpMesh->mPrimitiveTypes & aiPrimitiveType_TRIANGLE) ||
            assimpMesh->mNumVertices <= 0) {
            continue ;
        }
        
        Mesh* mesh = new Mesh();
        Material* material;
        
        if (assimpMesh->mMaterialIndex >= _materials.size()) {
            material = _defaultMaterial;
        } else {
            material = _materials[assimpMesh->mMaterialIndex];
        }
        
        GeometricPrimitive* primitive = new GeometricPrimitive(mesh, material);

        // Load vertices
        int verticesCount = assimpMesh->mNumVertices;
        Vertex* vertices = new Vertex[verticesCount];
        for (uint j = 0; j < assimpMesh->mNumVertices; ++j) {
            vertices[j].position = importAssimpVec3(assimpMesh->mVertices[j]);
            if (assimpMesh->mNormals) {
                vertices[j].normal = importAssimpVec3(assimpMesh->mNormals[j]);
            }
            if (assimpMesh->mTextureCoords[0]) {
                vertices[j].texCoord = vec2(assimpMesh->mTextureCoords[0][j].x,
                                                  assimpMesh->mTextureCoords[0][j].y);
            }
            if (assimpMesh->mTangents) {
                vertices[j].tangentU = importAssimpVec3(assimpMesh->mTangents[j]);
            }
        }
        
        // Load triangles
        int trianglesCount = 0;
        for (uint j = 0; j < assimpMesh->mNumFaces; ++j) {
            if (assimpMesh->mFaces[j].mNumIndices == 3) {
                trianglesCount += 1;
                _trianglesCount += 1;
            }
        }
        Triangle* triangles = new Triangle[trianglesCount];
        uint faceId = 0;
        for (uint j = 0; j < assimpMesh->mNumFaces; ++j) {
            if (assimpMesh->mFaces[j].mNumIndices == 3) {
                Vertex* a = &vertices[assimpMesh->mFaces[j].mIndices[0]];
                Vertex* b = &vertices[assimpMesh->mFaces[j].mIndices[1]];
                Vertex* c = &vertices[assimpMesh->mFaces[j].mIndices[2]];
                triangles[faceId] = Triangle(a, b, c);
                triangles[faceId].setMesh(mesh);
                ++faceId;
            }
        }
        
        mesh->setVertices(verticesCount, vertices);
        mesh->setTriangles(trianglesCount, triangles);
        
        // Create transformed primitive
        Transform transform(matrix);
        TransformedPrimitive* transformed = new TransformedPrimitive(primitive, transform);

        if (!name.empty()) {
            transformed->setName(name);
        } else {
            std::stringstream ss;
            ss << "Primitive " << primitive->getPrimitiveId();
            transformed->setName(ss.str());
        }
        
        // Apply custom setup to primitive
        if (_primitivesCallback) {
            if (!_primitivesCallback(transformed, primitive)) {
                continue;
            }
        }
        
        // Add primitive to scene
        *aggregate << transformed;
    }
    
    // Load child nodes recursively
    for (uint i = 0; i < assimpNode->mNumChildren; ++i) {
        importAssimpNode(aggregate, assimpScene, assimpNode->mChildren[i], camera, matrix);
    }
    
    return true;
}

vec3 AssimpImporter::importAssimpVec3(const aiVector3D& v) {
    return vec3(v.x, v.y, v.z);
}