//
//  SceneNode.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 6/4/14.
//
//

#include "SceneNode.h"

SceneNode::SceneNode() {
    
}

SceneNode::~SceneNode() {
    
}

void SceneNode::setAnimatedTransform(const AnimatedTransform& transform) {
    _transform = transform;
}

const AnimatedTransform& SceneNode::getAnimatedTransform() const {
    return _transform;
}

void SceneNode::setTransform(const Transform& transform) {
    _transform.setTransform(transform);
}

const Transform& SceneNode::getTransform() const {
    return _transform[0];
}