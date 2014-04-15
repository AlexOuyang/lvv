//
//  TransformedPrimitive.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/14/14.
//
//

#include "TransformedPrimitive.h"

#include "Ray.h"
#include "Intersection.h"

TransformedPrimitive::TransformedPrimitive(Primitive* primitive, const Transform& transform) :
_primitive(primitive),
_primitiveToWorld(transform),
_worldToPrimitive(Transform::Inverse(transform)) {
    
}

TransformedPrimitive::~TransformedPrimitive() {
}

bool TransformedPrimitive::intersect(const Ray& ray, Intersection* intersection) const {
    Ray transformedRay = _worldToPrimitive(ray);
    
    if (!_primitive->intersect(transformedRay, intersection)) {
        return false;
    }
    
    // Transform intersection
    intersection->point = _primitiveToWorld(intersection->point);
    intersection->normal = _primitiveToWorld.applyToVector(intersection->normal);
    intersection->t = transformedRay.tmax;
    
    // Transform ray
    ray.tmax = intersection->t;
    
    return true;
}

bool TransformedPrimitive::intersectP(const Ray& ray) const {
    Ray transformedRay = _worldToPrimitive(ray);
    
    return _primitive->intersectP(transformedRay);
}

AABB TransformedPrimitive::getBoundingBox() const {
    return _primitiveToWorld(_primitive->getBoundingBox());
}