//
//  AABB.h
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/14/14.
//
//

#ifndef __CSE168_Rendering__AABB__
#define __CSE168_Rendering__AABB__

#include "Core/Core.h"

class AABB {
public:
    
    static AABB Union(const AABB& box, const vec3& point);
    static AABB Union(const AABB& b1, const AABB& b2);
    
    AABB();
    AABB(const vec3& p);
    AABB(const vec3& p1, const vec3& p2);
    
    ~AABB();
    
    bool    intersectP(const Ray& ray, float* t0, float* t1);
    int     getMaxDimension() const;
    
    vec3 min;
    vec3 max;
};

#endif /* defined(__CSE168_Rendering__AABB__) */