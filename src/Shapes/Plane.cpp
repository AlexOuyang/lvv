//
//  Plane.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/5/14.
//
//

#include "Plane.h"

#include "Core/Ray.h"

Plane::Plane() : _origin(), _normal(), _d() {
    setNormal(vec3(0.f, 1.f, 0.f));
}

Plane::~Plane() {
    
}

void Plane::setOrigin(const vec3& origin) {
    _origin = origin;
    _d = dot(_normal, _origin);
}

void Plane::setNormal(const vec3& normal, float textureScaleX, float textureScaleY) {
    _normal = normalize(normal);
    _d = dot(_normal, _origin);
    
    // Set default values for s and t, the texture coordinates axis
    if (abs(_normal.y) > 1.0f-0.00001f) {
        _s = vec3(1.0f, 0.0f, 0.0f);
        _t = vec3(0.0f, 0.0f, 1.0f);
    } else {
        _s = normalize(cross(vec3(0.0f, 1.0f, 0.0f), _normal));
        _t = cross(_normal, _s);
    }
    _s *= textureScaleX;
    _t *= textureScaleY;
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

    // Generate tangents
    if (abs(_normal.y) > 1.0f-0.00001f) {
        intersection->tangentU = vec3(1.0f, 0.0f, 0.0f);
        intersection->tangentV = vec3(0.0f, 0.0f, 1.0f);
    } else {
        intersection->tangentU = normalize(cross(vec3(0.0f, 1.0f, 0.0f), _normal));
        intersection->tangentV = cross(_normal, intersection->tangentU);
    }
    
    // Compute uv coordinates
    vec3 z = cross(_s, _t);
    float z2 = dot(z, z);
    vec3 sprim = cross(_t, z) / z2;
    vec3 tprim = cross(z, _s) / z2;
    
    intersection->uv.s = dot((intersection->point - _origin), sprim);
    intersection->uv.t = dot((intersection->point - _origin), tprim);
    
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