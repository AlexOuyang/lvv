//
//  Glossy.h
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/24/14.
//
//

#ifndef __CSE168_Rendering__Glossy__
#define __CSE168_Rendering__Glossy__

#include "Core/Material.h"

class Glossy : public Material {
public:
    
    Glossy();
    ~Glossy();
    
    virtual Spectrum evaluateBSDF(const vec3& wo, const vec3& wi,
                                  const Intersection& intersection) const;
    virtual Spectrum sampleBSDF(const vec3& wo, vec3* wi, const Intersection& intersection,
                                BxDFType type) const;
    
    Spectrum color;
    float indexIn;
    float indexOut;
    float roughness;
};

#endif /* defined(__CSE168_Rendering__Glossy__) */
