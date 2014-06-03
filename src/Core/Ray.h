//
//  Ray.h
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/3/14.
//
//

#ifndef CSE168_Rendering_Ray_h
#define CSE168_Rendering_Ray_h

#include "Core.h"

class Ray {
public:
    
    enum Type {
        Primary             = 1 << 0,
        DiffuseReflected    = 1 << 1,
        SpecularReflected   = 1 << 2
    };
    
    Ray();
    Ray(const Ray& ray);
    ~Ray();
    
    vec3 operator()(float t) const;
    
    vec3    origin;
    vec3    direction;
    
    float           tmin;
    mutable float   tmax;
    int             depth;
    float           time;
    Type            type;
};

#endif
