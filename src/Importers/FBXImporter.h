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
#include "Core/AnimationEvaluator.h"
#include "Cameras/PerspectiveCamera.h"
#include "Shapes/AnimatedMesh.h"

class FBXImporter : public SceneImporter, public std::enable_shared_from_this<FBXImporter> {
public:
    
    static std::shared_ptr<FBXImporter> Load(const rapidjson::Value& value);
    
    FBXImporter();
    virtual ~FBXImporter();
    
    virtual bool import(Scene& scene);
    
    void importNode(FbxNode* fbxNode, Scene& scene,
                    const Transform& parentTransform=Transform());
    void importNodeAttribute(FbxNode* fbxNode,
                             FbxNodeAttribute* fbxAttribute,
                             Scene& scene, const Transform& transform);
    
    void importMesh(FbxNode* fbxNode, FbxMesh* fbxMesh,
                    Scene& scene, const Transform& transform);
    
    static vec3                     importVec3(const FbxVector4& v);
    static vec2                     importVec2(const FbxVector2& v);
    static mat4x4                   importMatrix(const FbxAMatrix& m);
    static Transform                importNodeTransform(const FbxNode* node);
    static std::shared_ptr<Texture> importTexture(FbxTexture* fbxTexture,
                                                  bool isFloat=false);
    static bool                     isNodeAnimated(FbxNode* fbxNode);
    
    class CameraAnimationEvaluator : public AnimationEvaluator {
    public:
        CameraAnimationEvaluator(FbxCamera* fbxCamera,
                                 const std::shared_ptr<PerspectiveCamera>& camera,
                                 const std::shared_ptr<FBXImporter>& importer);
        virtual ~CameraAnimationEvaluator();
        
        virtual void evaluate(float tstart, float tend);
        
        FbxCamera*                          fbxCamera;
        std::shared_ptr<PerspectiveCamera>  camera;
        std::shared_ptr<FBXImporter>        importer;
    };
    
    class MeshAnimationEvaluator : public AnimationEvaluator {
    public:
        MeshAnimationEvaluator(FbxNode* fbxNode,
                               const std::shared_ptr<TransformedPrimitive>& tp,
                               const std::shared_ptr<FBXImporter>& importer);
        virtual ~MeshAnimationEvaluator();
        
        virtual void evaluate(float tstart, float tend);
        
        FbxNode*                                fbxNode;
        std::shared_ptr<TransformedPrimitive>   transformedPrimitive;
        std::shared_ptr<FBXImporter>            importer;
    };
    
    class SkinnedMeshAnimationEvaluator : public AnimationEvaluator {
    public:
        SkinnedMeshAnimationEvaluator(FbxMesh* fbxMesh,
                                      const std::shared_ptr<AnimatedMesh>& mesh,
                                      const std::shared_ptr<Aggregate>& aggregate,
                                      const std::shared_ptr<FBXImporter>& importer);
        virtual ~SkinnedMeshAnimationEvaluator();
        
        virtual void evaluate(float tstart, float tend);
        
        FbxMesh*                        fbxMesh;
        std::shared_ptr<AnimatedMesh>   mesh;
        std::shared_ptr<Aggregate>      aggregate;
        std::shared_ptr<FBXImporter>    importer;
    };

    
private:
    
    typedef std::pair<ImportedMaterialAttributes, std::shared_ptr<Material>> ImportedMaterial;
    
    void _importNodeMaterials(std::vector<ImportedMaterial>& materials,
                              const FbxNode *fbxNode, Scene &scene);
    
    bool _loadMeshData(FbxMesh* fbxMesh, uint_t* verticesCount, Vertex** vertices,
                       uint_t* indicesCount, uint_t** indices,
                       uint_t** materialIndices, bool* hasUVs,
                       const FbxTime& time) const;
    bool _applyMeshSkinning(FbxMesh* fbxMesh, uint_t* controlPointsCount,
                            vec3** controlPoints, vec3** normals,
                            const FbxTime& time) const;
    
    FbxManager*                                     _fbxManager;
    std::map<FbxSurfaceMaterial*,ImportedMaterial>  _importedMaterials;
};

#endif /* defined(__CSE168_Rendering__FBXImporter__) */
