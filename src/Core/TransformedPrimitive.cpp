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

TransformedPrimitive::TransformedPrimitive(const std::shared_ptr<Primitive>& primitive,
                                           const Transform& transform) :
_primitive(primitive), _material(nullptr) {
    _transform.setTransform(transform);
}

TransformedPrimitive::TransformedPrimitive(const std::shared_ptr<Primitive>& primitive,
                                           const AnimatedTransform& transform) :
_primitive(primitive), _material(nullptr) {
    if (transform.isActuallyAnimated()) {
        _transform = transform;
    } else {
        _transform.setTransform(transform[0]);
    }
}


TransformedPrimitive::~TransformedPrimitive() {
}

void TransformedPrimitive::setPrimitive(const std::shared_ptr<Primitive>& primitive) {
    _primitive = primitive;
}

std::shared_ptr<Primitive> TransformedPrimitive::getPrimitive() const {
    return _primitive;
}

void TransformedPrimitive::setMaterial(Material* material) {
    _material = material;
}

bool TransformedPrimitive::canIntersect() const {
    return _primitive->canIntersect();
}

bool TransformedPrimitive::intersect(const Ray& ray, Intersection* intersection) const {
    Transform primitiveToWorld = _transform.interpolate(ray.time);
    Transform worldToPrimitive = Transform::Inverse(primitiveToWorld);
    Ray transformedRay = worldToPrimitive(ray);
    
    if (!_primitive->intersect(transformedRay, intersection)) {
        return false;
    }
    
    // Transform intersection
    
    intersection->point = primitiveToWorld(intersection->point);
    // Transform normal with inverse transpose of transformation matrix
    mat4x4 normalMatrix = primitiveToWorld.getMatrix();
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
    Transform primitiveToWorld = _transform.interpolate(ray.time);
    Transform worldToPrimitive = Transform::Inverse(primitiveToWorld);
    Ray transformedRay = worldToPrimitive(ray);
    
    return _primitive->intersectP(transformedRay);
}

AABB TransformedPrimitive::getBoundingBox() const {
    return _transform.motionBounds(_primitive->getBoundingBox(), false);
}