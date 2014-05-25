//
//  DensityVolume.h
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 5/24/14.
//
//

#ifndef __CSE168_Rendering__DensityVolume__
#define __CSE168_Rendering__DensityVolume__

#include "Core/Core.h"
#include "Core/Volume.h"

class DensityVolume : public Volume {
public:
    
    DensityVolume(const AABB& bounds);
    virtual ~DensityVolume();
    
    void setBounds(const AABB&& bounds);
    void setSigmaA(const Spectrum& sa);
    void setSigmaS(const Spectrum& ss);
    void setLe(const Spectrum& le);
    void setPhaseParameter(float g);
    
    virtual float density(const vec3& p) const;
    
    virtual AABB getBoundingBox() const;
    virtual bool intersectP(const Ray& ray, float* t0, float* t1) const;
    virtual Spectrum sigmaA(const vec3& p) const;
    virtual Spectrum sigmaS(const vec3& p) const;
    virtual Spectrum le(const vec3& p) const;
    virtual float phase(const vec3& p, const vec3& wi, const vec3& wo) const;
    virtual Spectrum sigmaT(const vec3& p) const;
    virtual Spectrum tau(const Ray& ray) const;
    
private:
    AABB    _bounds;
    Spectrum    _sigmaA;
    Spectrum    _sigmaS;
    Spectrum    _le;
    float       _g;
};

#endif /* defined(__CSE168_Rendering__DensityVolume__) */
