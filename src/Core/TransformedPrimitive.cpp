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
_primitive(primitive), _material(nullptr),
_worldToPrimitive() {
    _worldToPrimitive.setTransform(Transform::Inverse(transform));
}

TransformedPrimitive::TransformedPrimitive(Primitive* primitive,
                                           const AnimatedTransform& transform) :
_primitive(primitive), _material(nullptr),
_worldToPrimitive() {
    if (transform.isActuallyAnimated()) {
        _worldToPrimitive.setTransforms(Transform::Inverse(transform[0]),
                                        Transform::Inverse(transform[1]));
    } else {
        _worldToPrimitive.setTransform(Transform::Inverse(transform[0]));
    }
}


TransformedPrimitive::~TransformedPrimitive() {
}

void TransformedPrimitive::setPrimitive(Primitive* primitive) {
    _primitive = primitive;
}

Primitive* TransformedPrimitive::getPrimitive() const {
    return _primitive;
}

void TransformedPrimitive::setMaterial(Material* material) {
    _material = material;
}

void TransformedPrimitive::setTransform(const Transform& transform) {
    _worldToPrimitive.setTransform(Transform::Inverse(transform));
}

void TransformedPrimitive::setTransform(const AnimatedTransform& transform) {
    _worldToPrimitive.setTransforms(Transform::Inverse(transform[0]),
                                    Transform::Inverse(transform[1]));
}

Transform TransformedPrimitive::getTransform() const {
    return Transform::Inverse(_worldToPrimitive[0]);
}

bool TransformedPrimitive::canIntersect() const {
    return _primitive->canIntersect();
}

bool TransformedPrimitive::intersect(const Ray& ray, Intersection* intersection) const {
    Transform worldToPrimitive = _worldToPrimitive.interpolate(ray.time);
    Ray transformedRay = worldToPrimitive(ray);
    
    if (!_primitive->intersect(transformedRay, intersection)) {
        return false;
    }
    
    // Transform intersection
    Transform primitiveToWorld = Transform::Inverse(worldToPrimitive);
    
    intersection->point = primitiveToWorld(intersection->point);
    // Transform normal with inverse transpose of transformation matrix
    mat4x4 normalMatrix = primitiveToWorld.m;
    intersection->normal = normalize(vec3(normalMatrix * vec4(intersection->normal, 0.0f)));
    intersection->tangentU = normalize(vec3(normalMatrix * vec4(intersection->tangentU, 0.0f)));
    intersection->tangentV = normalize(vec3(normalMatrix * vec4(intersection->tangentV, 0.0f)));
    intersection->t = transformedRay.tmax;
    
    // Transform ray
    ray.tmax = intersection->t;
    
    // Set material if specified
    if (_material) {
        intersection->material = _material;
    }
    
    return true;
}

bool TransformedPrimitive::intersectP(const Ray& ray) const {
    Transform worldToPrimitive = _worldToPrimitive.interpolate(ray.time);
    Ray transformedRay = worldToPrimitive(ray);
    
    return _primitive->intersectP(transformedRay);
}

AABB TransformedPrimitive::getBoundingBox() const {
    return _worldToPrimitive.motionBounds(_primitive->getBoundingBox(), true);
}