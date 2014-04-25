//
//  Metal.h
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/23/14.
//
//

#ifndef __CSE168_Rendering__Metal__
#define __CSE168_Rendering__Metal__

#include "Core/Material.h"

class Metal : public Material {
public:
    
    Metal();
    ~Metal();
    
    virtual Spectrum evaluateBSDF(const vec3& wo, const vec3& wi,
                                  const Intersection& intersection) const;
    virtual Spectrum sampleBSDF(const vec3& wo, vec3* wi, const Intersection& intersection,
                                BxDFType type) const;
    
    float       eta;
    float       k;
    float       roughness;
    Spectrum    color;
};

#endif /* defined(__CSE168_Rendering__Metal__) */
