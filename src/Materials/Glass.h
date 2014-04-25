//
//  Glass.h
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/24/14.
//
//

#ifndef __CSE168_Rendering__Glass__
#define __CSE168_Rendering__Glass__

#include "Core/Material.h"

class Glass : public Material {
public:
    
    Glass();
    ~Glass();
    
    virtual Spectrum evaluateBSDF(const vec3& wo, const vec3& wi,
                                  const Intersection& intersection) const;
    virtual Spectrum sampleBSDF(const vec3& wo, vec3* wi, const Intersection& intersection,
                                BxDFType type) const;
    
    virtual Spectrum transmittedLight(float distance) const;
    
    float       indexIn;
    float       indexOut;
    vec3        absorptionColor;
    float       absorptionCoeff;
    float       roughness;
};

#endif /* defined(__CSE168_Rendering__Glass__) */
