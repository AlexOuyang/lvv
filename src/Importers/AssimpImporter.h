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

class AssimpImporter : public SceneImporter {
public:
        
    static std::shared_ptr<AssimpImporter> Load(const rapidjson::Value& value);
    
    AssimpImporter();
    virtual ~AssimpImporter();
    
    virtual bool import(Scene& scene);
    
    void importMaterials(const aiScene* assimpScene, Scene& scene);
    void importNode(const aiScene* assimpScene, aiNode* assimpNode,
                    Scene& scene, const mat4x4& parentMatrix=mat4x4());
    
    mat4x4  importMatrix(const aiMatrix4x4& matrix);
    vec3    importVec3(const aiVector3D& v);
    
private:
    Assimp::Importer                        _importer;
    uint_t                                  _trianglesCount;
    std::vector<std::shared_ptr<Material>>  _importedMaterials;
   
    /*
    enum ShadingMode {
        Lambert,
        Phong
    };
    
    struct MaterialAttributes {
        std::string name;
        std::string texturePath;
        vec3        color;
        vec3        transparency;
        ShadingMode shadingMode;
    };
    
    enum LightType {
        DirectionalLight,
        PointLight
    };
    
    struct LightAttributes {
        LightType   type;        
        vec3        position;
        vec3        direction;
        vec3        color;
        float       intensity;
    };
    
    typedef std::function<std::shared_ptr<Material> (const MaterialAttributes&)> MaterialCallback;
    typedef std::function<bool (const std::shared_ptr<TransformedPrimitive>&,
                                const std::shared_ptr<GeometricPrimitive>&)> PrimitiveCallback;
    typedef std::function<void (const LightAttributes&)> LightCallback;
    
    void setDefaultMaterial(const std::shared_ptr<Material>& material);
    void setMaterialCallback(MaterialCallback callback);
    void setPrimitivesCallback(PrimitiveCallback callback);
    void setLightsCallback(LightCallback callback);
    
    bool importModel(const std::shared_ptr<Aggregate>& aggregate, const std::string&
                     filename, std::shared_ptr<Camera>* camera=nullptr);
    
    bool importAssimpMaterials(const aiScene* assimpScene);
    bool importAssimpNode(const std::shared_ptr<Aggregate>& aggregate,
                          const aiScene* assimpScene,
                          aiNode* assimpNode, std::shared_ptr<Camera>* camera=nullptr,
                          const mat4x4& parentMatrix=mat4x4());
    
    vec3 importAssimpVec3(const aiVector3D& v);
    
private:
    Assimp::Importer                        _importer;
    std::vector<std::shared_ptr<Material>>  _materials;
    int                                     _trianglesCount;
    MaterialCallback                        _materialCallback;
    std::shared_ptr<Material>               _defaultMaterial;
    PrimitiveCallback                       _primitivesCallback;
    LightCallback                           _lightsCallback;
*/
};

#endif /* defined(__CSE168_Rendering__AssimpImporter__) */