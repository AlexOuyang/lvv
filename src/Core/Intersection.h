//
//  Intersection.h
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/3/14.
//
//

#ifndef __CSE168_Rendering__Intersection__
#define __CSE168_Rendering__Intersection__

#include "Core.h"
#include "Material.h"

class Intersection {
public:
    
    Intersection();
    ~Intersection();
    
    float       t;
    float       rayEpsilon;
    vec3        point;
    vec3        normal;
    vec2        uv;
    Material*   material;
};

#endif /* defined(__CSE168_Rendering__Intersection__) */
