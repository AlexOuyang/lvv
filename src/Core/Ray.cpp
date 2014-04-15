//
//  Ray.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/15/14.
//
//

#include "Ray.h"

Ray::Ray() : origin(), direction(), tmin(Core::Epsilon), tmax(INFINITY), depth(0) {
    
}

Ray::~Ray() {
    
}

vec3 Ray::operator()(float t) const {
    return origin + t * direction;
}