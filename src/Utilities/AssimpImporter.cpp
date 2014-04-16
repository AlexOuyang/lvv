//
//  AssimpImporter.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/5/14.
//
//

#include <iostream>

#include "AssimpImporter.h"

#include "Core/GeometricPrimitive.h"
#include "Shapes/Vertex.h"
#include "Shapes/Mesh.h"
#include "Materials/MatteMaterial.h"

AssimpImporter::AssimpImporter()
: _importer(), _defaultMaterial(), _materials(), _trianglesCount() {
    
}

AssimpImporter::~AssimpImporter() {
    
}

void AssimpImporter::setDefaultMaterial(Material *material) {
    _defaultMaterial = material;
}

bool AssimpImporter::importModel(Aggregate* aggregate, const std::string& filename) {
    // Read the scene from the file
    const aiScene* assimpScene = _importer.ReadFile(filename,
                                              aiProcess_Triangulate
                                              | aiProcess_GenSmoothNormals
                                              | aiProcess_FixInfacingNormals);
    
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
    bool status = importAssimpNode(aggregate, assimpScene, assimpScene->mRootNode);
    
    std::cout << "AssimImporter: loaded " << _trianglesCount << " triangles" << std::endl;
    
    return status;
}

bool AssimpImporter::importAssimpMaterials(const aiScene* assimpScene) {
    _materials.clear();
    for (uint i = 0; i < assimpScene->mNumMaterials; ++i) {
        aiMaterial* assimpMaterial = assimpScene->mMaterials[i];
        MatteMaterial* material = new MatteMaterial();
        
        aiColor3D aiColor;
        
        if (assimpMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, aiColor) != -1) {
            material->setColor(vec3(aiColor.r, aiColor.g, aiColor.b));
        }
        
        _materials.push_back(material);
    }
    return true;
}

bool AssimpImporter::importAssimpNode(Aggregate* aggregate, const aiScene* assimpScene,
                                      aiNode* assimpNode) {
    
    std::string name = assimpNode->mName.C_Str();
    
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
    
        if (!name.empty()) {
            primitive->name = name;
        }
        
        // Load vertices
        mesh->verticesCount = assimpMesh->mNumVertices;
        mesh->vertices = new Vertex[mesh->verticesCount];
        for (uint j = 0; j < assimpMesh->mNumVertices; ++j) {
            mesh->vertices[j].position = vec3(assimpMesh->mVertices[j].x,
                                               assimpMesh->mVertices[j].y,
                                               assimpMesh->mVertices[j].z);
            if (assimpMesh->mNormals) {
                mesh->vertices[j].normal = vec3(assimpMesh->mNormals[j].x,
                                                  assimpMesh->mNormals[j].y,
                                                  assimpMesh->mNormals[j].z);
            }
            if (assimpMesh->mTextureCoords[0]) {
                mesh->vertices[j].texCoord = vec2(assimpMesh->mTextureCoords[0][j].x,
                                                  assimpMesh->mTextureCoords[0][j].y);
            }
        }
        
        // Load triangles
        mesh->trianglesCount = 0;
        for (uint j = 0; j < assimpMesh->mNumFaces; ++j) {
            if (assimpMesh->mFaces[j].mNumIndices == 3) {
                mesh->trianglesCount += 1;
                _trianglesCount += 1;
            }
        }
        mesh->triangles = new Triangle[mesh->trianglesCount];
        uint faceId = 0;
        for (uint j = 0; j < assimpMesh->mNumFaces; ++j) {
            if (assimpMesh->mFaces[j].mNumIndices == 3) {
                Vertex* a = &mesh->vertices[assimpMesh->mFaces[j].mIndices[0]];
                Vertex* b = &mesh->vertices[assimpMesh->mFaces[j].mIndices[1]];
                Vertex* c = &mesh->vertices[assimpMesh->mFaces[j].mIndices[2]];
                mesh->triangles[faceId] = Triangle(a, b, c);
                ++faceId;
            }
        }
        
        // Add mesh to scene
        *aggregate << primitive;
    }
    
    // Load child nodes recursively
    for (uint i = 0; i < assimpNode->mNumChildren; ++i) {
        importAssimpNode(aggregate, assimpScene, assimpNode->mChildren[i]);
    }
    
    return true;
}