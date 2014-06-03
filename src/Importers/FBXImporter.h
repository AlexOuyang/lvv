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
    
    void importNode(const FbxNode* node, Scene& scene);
    void importNodeAttribute(const FbxNode* node,
                             const FbxNodeAttribute* attribute, Scene& scene);
    
    vec3 importVec3(const FbxVector4& v);
    vec2 importVec2(const FbxVector2& v);
    
private:
    FbxManager* _fbxManager;
};

#endif /* defined(__CSE168_Rendering__FBXImporter__) */
