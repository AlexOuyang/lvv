//
//  Volume.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 5/21/14.
//
//

#include "Volume.h"

float Volume::PhaseHenyeyGreenstein(const vec3& wi, const vec3& wo, float g) {
    // Henyey-Greenstein phase function
    float cosTheta = dot(wi, wo);
    float g2 = g*g;
    return (
            (1.f - g2) / (4.f*M_PI*powf(1.f + g2 - 2.f*g*cosTheta, 1.5f))
            );
}
