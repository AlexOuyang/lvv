//
//  Sphere.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/5/14.
//
//

#include "Sphere.h"

#include "Core/Ray.h"

Sphere::Sphere() {
    
}

Sphere::~Sphere() {
    
}

void Sphere::setCenter(const vec3& center) {
    _center = center;
}

void Sphere::setRadius(float radius) {
    _radius = radius;
}

bool Sphere::intersect(const Ray& ray, Intersection* intersection) const {
    float t = -dot((ray.origin - _center), ray.direction);
    vec3 q = ray(t);
    vec3 qc = q - _center;
    float qc2 = dot(qc, qc);
    
    if (qc2 > _radius*_radius) {
        return false;
    }
    
    float a = sqrt(_radius*_radius - qc2);

    if (t - a >= ray.tmin) {
        t = t - a;
    } else {
        t = t + a;
    }

    if (t < ray.tmin || t > ray.tmax) {
        return false;
    }
    
    vec3 point = ray.origin + t*ray.direction;
    
    // Update the ray
    ray.tmax = t;
    
    // Fill in intersection informations
    intersection->t = t;
    intersection->point = point;
    intersection->normal = normalize(point - _center);
    intersection->rayEpsilon = 5e-4f * t;
    return true;
}

bool Sphere::intersectP(const Ray& ray) const {
    float t = -dot((ray.origin - _center), ray.direction);
    vec3 q = ray(t);
    vec3 qc = q - _center;
    float qc2 = dot(qc, qc);
    
    if (qc2 > _radius*_radius) {
        return false;
    }
    
    float a = sqrt(_radius*_radius - qc2);
    
    if (t - a >= ray.tmin) {
        t = t - a;
        
    } else {
        t = t + a;
    }
    
    if (t < ray.tmin || t > ray.tmax) {
        return false;
    }
    return true;
}

AABB Sphere::getBoundingBox() const {
    vec3 r = vec3(_radius, _radius, _radius);
    
    return AABB(_center - r, _center + r);
}