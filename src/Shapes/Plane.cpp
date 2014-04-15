//
//  Plane.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/5/14.
//
//

#include "Plane.h"

#include "Core/Ray.h"

Plane::Plane() : _origin(), _normal(0, 1, 0), _d() {
    _d = dot(_normal, _origin);
}

Plane::~Plane() {
    
}

void Plane::setOrigin(const vec3& origin) {
    _origin = origin;
    _d = dot(_normal, _origin);
}

void Plane::setNormal(const vec3& normal) {
    _normal = normalize(normal);
    _d = dot(_normal, _origin);
}

bool Plane::intersect(const Ray& ray, Intersection* intersection) const {
    float dn = dot(ray.direction, _normal);
    
    if (dn == 0) {
        return false;
    }
    
    float t = (_d - dot(ray.origin, _normal)) / dn;
    if (t < ray.tmin || t > ray.tmax) {
        return false;
    }
    
    // Update the ray
    ray.tmax = t;
    
    // Fill in intersection informations
    intersection->t = t;
    intersection->point = ray(t);
    intersection->normal = _normal;
    intersection->rayEpsilon = 5e-4f * t;
    return true;
}

bool Plane::intersectP(const Ray& ray) const {
    float dn = dot(ray.direction, _normal);
    
    if (dn == 0) {
        return false;
    }
    
    float t = (_d - dot(ray.origin, _normal)) / dn;
    if (t < ray.tmin || t > ray.tmax) {
        return false;
    }
    
    return true;
}

AABB Plane::getBoundingBox() const {
    return AABB();
}