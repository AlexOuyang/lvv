//
//  AssimpImporter.h
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/5/14.
//
//

#ifndef __CSE168_Rendering__AssimpImporter__
#define __CSE168_Rendering__AssimpImporter__

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>

#include "Core/Scene.h"
#include "Core/Material.h"

class AssimpImporter {
public:
    
    AssimpImporter();
    ~AssimpImporter();
    
    void setDefaultMaterial(Material* material);
    
    bool importModel(Aggregate* aggregate, const std::string& filename);
    
    bool importAssimpMaterials(const aiScene* assimpScene);
    bool importAssimpNode(Aggregate* aggregate, const aiScene* assimpScene, aiNode* assimpNode);
    
private:
    Assimp::Importer        _importer;
    Material*               _defaultMaterial;
    std::vector<Material*>  _materials;
    int                     _trianglesCount;
};

#endif /* defined(__CSE168_Rendering__AssimpImporter__) */
