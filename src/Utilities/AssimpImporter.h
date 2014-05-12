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
#include "Core/Camera.h"
#include "Core/TransformedPrimitive.h"
#include "Core/GeometricPrimitive.h"
#include "Utilities/ImageLoading.h"

class AssimpImporter {
public:
    
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
    
    struct LightAttributes {
        vec3    position;
        vec3    direction;
        vec3    color;
        float   intensity;
    };
    
    typedef std::function<Material* (const MaterialAttributes&)> MaterialCallback;
    typedef std::function<bool (TransformedPrimitive*, GeometricPrimitive*)> PrimitiveCallback;
    typedef std::function<void (const LightAttributes&)> LightCallback;
    
    AssimpImporter();
    ~AssimpImporter();
    
    void setDefaultMaterial(Material* material);
    void setMaterialCallback(MaterialCallback callback);
    void setPrimitivesCallback(PrimitiveCallback callback);
    void setLightsCallback(LightCallback callback);
    
    bool importModel(Aggregate* aggregate, const std::string& filename, Camera** camera=nullptr);
    
    bool importAssimpMaterials(const aiScene* assimpScene);
    bool importAssimpNode(Aggregate* aggregate, const aiScene* assimpScene,
                          aiNode* assimpNode, Camera** camera=nullptr,
                          const mat4x4& parentMatrix=mat4x4());
    
    vec3 importAssimpVec3(const aiVector3D& v);
    
private:
    Assimp::Importer        _importer;
    std::vector<Material*>  _materials;
    int                     _trianglesCount;
    MaterialCallback        _materialCallback;
    Material*               _defaultMaterial;
    PrimitiveCallback       _primitivesCallback;
    LightCallback           _lightsCallback;
};

#endif /* defined(__CSE168_Rendering__AssimpImporter__) */