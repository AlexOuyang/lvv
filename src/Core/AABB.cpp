//
//  AABB.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/14/14.
//
//

#include <algorithm>

#include "AABB.h"

#include "Core/Ray.h"

AABB AABB::Union(const AABB &b, const vec3 &p) {
    AABB ret = b;

    ret.min.x = glm::min(b.min.x, p.x);
    ret.min.y = glm::min(b.min.y, p.y);
    ret.min.z = glm::min(b.min.z, p.z);
    ret.max.x = glm::max(b.max.x, p.x);
    ret.max.y = glm::max(b.max.y, p.y);
    ret.max.z = glm::max(b.max.z, p.z);
    return ret;
}

AABB AABB::Union(const AABB &b, const AABB &b2) {
    AABB ret;

    ret.min.x = glm::min(b.min.x, b2.min.x);
    ret.min.y = glm::min(b.min.y, b2.min.y);
    ret.min.z = glm::min(b.min.z, b2.min.z);
    ret.max.x = glm::max(b.max.x, b2.max.x);
    ret.max.y = glm::max(b.max.y, b2.max.y);
    ret.max.z = glm::max(b.max.z, b2.max.z);
    return ret;
}


AABB::AABB() : min(INFINITY, INFINITY, INFINITY), max(-INFINITY, -INFINITY, -INFINITY) {
    
}

AABB::AABB(const vec3& p) : min(p), max(p) {
    
}

AABB::AABB(const vec3& p1, const vec3& p2)
: min(glm::min(p1, p2)), max(glm::max(p1, p2)) {
    
}

AABB::~AABB() {
    
}

bool AABB::intersectP(const Ray& ray, float* t0, float* t1) {
    float hit0 = ray.tmin;
    float hit1 = ray.tmax;
    
    // Compute intersection intervals for each box axis
    for (int i = 0; i < 3; ++i) {
        // Compute the interval, formula for dimension x: (ti = (xi - ox) / ox)
        float invDir = 1.0f / ray.direction[i];
        float tNear = (min[i] - ray.origin[i]) * invDir;
        float tFar = (max[i] - ray.origin[i]) * invDir;
        
        // Swap values if needed
        if (tNear > tFar) {
            std::swap(tNear, tFar);
        }
        
        // Reduce interval with computed values
        hit0 = tNear > hit0 ? tNear : hit0;
        hit1 = tFar < hit1 ? tFar : hit1;
        
        // If interval is degenerate, ray doesn't intersect
        if (hit0 > hit1) {
            return false;
        }
    }
    
    *t0 = hit0;
    *t1 = hit1;
    return true;
}

int AABB::getMaxDimension() const {
    vec3 delta = max - min;
    if (delta.x > delta.y && delta.x > delta.z) {
        return 0;
    } else if (delta.y > delta.z) {
        return 1;
    } else {
        return 2;
    }
}

float AABB::surfaceArea() const {
    vec3 delta = max - min;
    return 2.f * (delta.x * delta.y + delta.x * delta.z + delta.y * delta.z);
}