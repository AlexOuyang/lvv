//
//  Transform.h
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/13/14.
//
//

#ifndef __CSE168_Rendering__Transform__
#define __CSE168_Rendering__Transform__

#include "Core/Core.h"

#include "AABB.h"

class Transform {
public:
    
    static Transform Inverse(const Transform& t);
    
    Transform();
    Transform(const mat4x4& m);
    
    vec3 applyToVector(const vec3& p) const;
    vec3 operator()(const vec3& v) const;
    Ray operator()(const Ray& r) const;
    AABB operator()(const AABB& box) const;
    
    mat4x4 m;
};

#endif /* defined(__CSE168_Rendering__Transform__) */
