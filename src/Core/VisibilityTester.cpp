//
//  VisibilityTester.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/5/14.
//
//

#include "VisibilityTester.h"

#include "Scene.h"

#include <QDebug>

VisibilityTester::VisibilityTester() : _ray() {
    
}

VisibilityTester::~VisibilityTester() {
    
}

bool VisibilityTester::unoccluded(const Scene& scene) const {
    //return true;
    //qDebug() << _ray.tmax;
    return !scene.intersectP(_ray);
}

void VisibilityTester::setSegment(const vec3& p1, float epsilon, const vec3& p2) {
    _ray = Ray();
    _ray.origin = p1;
    _ray.direction = p2 - p1;
    _ray.tmin = epsilon;
    _ray.tmax = glm::length(_ray.direction) - epsilon;
    // Normalize direction
    _ray.direction = glm::normalize(_ray.direction);
}

void VisibilityTester::setRay(const vec3& origin, float epsilon, const vec3& direction) {
    _ray = Ray();
    _ray.origin = origin;
    _ray.direction = direction;
    _ray.tmin = epsilon;
    _ray.tmax = INFINITY;
}