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

AABB AABB::Union(const AABB &box, const vec3 &point) {
    return AABB(glm::min(box.min, point), glm::max(box.max, point));
}

AABB AABB::Union(const AABB &b1, const AABB &b2) {
    return AABB(glm::min(b1.min, b2.min), glm::max(b1.max, b2.max));
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