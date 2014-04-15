//
//  Material.h
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/5/14.
//
//

#ifndef CSE168_Rendering_Material_h
#define CSE168_Rendering_Material_h

#include "Spectrum.h"

class Intersection;

class Material {
public:
    
    enum BxDFType {
        BSDFReflection      = 1 << 0,
        BSDFSpecular        = 1 << 1,
        BSDFTransmission    = 1 << 2
    };
    
    virtual ~Material() {};
    
    virtual Spectrum evaluateBSDF(const vec3& wo, const vec3& wi) const = 0;
    virtual Spectrum sampleBSDF(const vec3& wo, vec3* wi, BxDFType type) const = 0;
};

#endif
