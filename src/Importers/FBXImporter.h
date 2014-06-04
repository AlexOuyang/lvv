//
//  FBXImporter.h
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 6/1/14.
//
//

#ifndef __CSE168_Rendering__FBXImporter__
#define __CSE168_Rendering__FBXImporter__

#include <fbxsdk.h>

#include "Core/Core.h"
#include "Core/SceneImporter.h"

class FBXImporter : public SceneImporter {
public:
    
    static std::shared_ptr<FBXImporter> Load(const rapidjson::Value& value);
    
    FBXImporter();
    virtual ~FBXImporter();
    
    virtual bool import(Scene& scene);
    
    void importNode(const FbxNode* fbxNode, Scene& scene,
                    const Transform& parentTransform=Transform());
    void importNodeAttribute(const FbxNode* fbxNode,
                             const FbxNodeAttribute* fbxAttribute,
                             Scene& scene, const Transform& transform);
    
    void importMesh(const FbxNode* fbxNode, const FbxMesh* fbxMesh,
                    Scene& scene, const Transform& transform);
    
    static vec3                     importVec3(const FbxVector4& v);
    static vec2                     importVec2(const FbxVector2& v);
    static Transform                importNodeTransform(const FbxNode* node);
    static std::shared_ptr<Texture> importTexture(FbxTexture* fbxTexture,
                                                  bool isFloat=false);
    
private:
    
    typedef std::pair<ImportedMaterialAttributes, std::shared_ptr<Material>> ImportedMaterial;
    
    ImportedMaterial _importNodeMaterial(const FbxNode *fbxNode, Scene &scene);
    
    void _loadMeshData(const FbxMesh* fbxMesh, uint_t* verticesCount, Vertex** vertices,
                       uint_t* indicesCount, uint_t** indices) const;
    
    FbxManager*                                     _fbxManager;
    std::map<FbxSurfaceMaterial*,ImportedMaterial>  _importedMaterials;
};

#endif /* defined(__CSE168_Rendering__FBXImporter__) */
