//
//  Vertex.h
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/4/14.
//
//

#ifndef __CSE168_Rendering__Vertex__
#define __CSE168_Rendering__Vertex__

#include "Core/Core.h"

struct Vertex {
    
    Vertex() {};
    Vertex(const vec3& p, const vec3& n, const vec2& tex)
    : position(p), normal(n), texCoord(tex) {}
    ~Vertex() {};
    
    vec3    position;
    vec3    normal;
    vec2    texCoord;
};

#endif /* defined(__CSE168_Rendering__Vertex__) */
