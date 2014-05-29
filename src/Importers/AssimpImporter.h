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

#include "Core/Core.h"
#include "Core/SceneImporter.h"
#include "Core/Scene.h"
#include "Core/Material.h"
#include "Core/Camera.h"
#include "Core/TransformedPrimitive.h"
#include "Core/GeometricPrimitive.h"
#include "Utilities/ImageLoading.h"
#include "Cameras/PerspectiveCamera.h"
#include "Lights/PointLight.h"
#include "Lights/DirectionalLight.h"

class AssimpImporter : public SceneImporter {
public:
        
    static std::shared_ptr<AssimpImporter> Load(const rapidjson::Value& value);
    
    AssimpImporter();
    virtual ~AssimpImporter();
    
    virtual bool import(Scene& scene);
    
    bool importCamera(const aiScene *assimpScene, aiNode* assimpNode,
                      const Transform& transform, Scene &scene);
    bool importLight(const aiScene* assimpScene, aiNode* assimpNode,
                     const Transform& transform, Scene& scene);
    void importMaterials(const aiScene* assimpScene, Scene& scene);
    void importNode(const aiScene* assimpScene, aiNode* assimpNode,
                    Scene& scene, const mat4x4& parentMatrix=mat4x4());
    
    mat4x4  importMatrix(const aiMatrix4x4& matrix);
    vec3    importVec3(const aiVector3D& v);
    vec3    importColor(const aiColor3D& c);
    
private:
    Assimp::Importer    _importer;
    uint_t              _trianglesCount;
    
    typedef std::pair<ImportedMaterialAttributes, std::shared_ptr<Material>> ImportedMaterial;
    std::vector<ImportedMaterial>   _importedMaterials;
};

#endif /* defined(__CSE168_Rendering__AssimpImporter__) */