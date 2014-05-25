//
//  Volume.h
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 5/21/14.
//
//

#ifndef __CSE168_Rendering__Volume__
#define __CSE168_Rendering__Volume__

#include "Core.h"
#include "AABB.h"
#include "Spectrum.h"

class Volume {
public:
    
    static float PhaseHenyeyGreenstein(const vec3& wi, const vec3& wo, float g);
    
    virtual ~Volume() {};
    virtual AABB getBoundingBox() const = 0;
    virtual bool intersectP(const Ray& ray, float* t0, float* t1) const = 0;
    virtual Spectrum sigmaA(const vec3& p) const = 0;
    virtual Spectrum sigmaS(const vec3& p) const = 0;
    virtual Spectrum le(const vec3& p) const = 0;
    virtual float phase(const vec3& p, const vec3& wi, const vec3& wo) const = 0;
    virtual Spectrum sigmaT(const vec3& p) const = 0;
    virtual Spectrum tau(const Ray& ray) const = 0;
};

#endif /* defined(__CSE168_Rendering__Volume__) */
