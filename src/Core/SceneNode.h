//
//  SceneNode.h
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 6/4/14.
//
//

#ifndef __CSE168_Rendering__SceneNode__
#define __CSE168_Rendering__SceneNode__

#include "Core.h"
#include "AnimatedTransform.h"

class SceneNode {
public:
    
    SceneNode();
    virtual ~SceneNode();
    
    void setAnimatedTransform(const AnimatedTransform& transform);
    const AnimatedTransform& getAnimatedTransform() const;
    
    void setTransform(const Transform& transform);
    const Transform& getTransform() const;
    
protected:
    AnimatedTransform   _transform;
};

#endif /* defined(__CSE168_Rendering__SceneNode__) */
