//
//  DirectionalLight.h
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/5/14.
//
//

#ifndef __CSE168_Rendering__DirectionalLight__
#define __CSE168_Rendering__DirectionalLight__

#include "Core/Light.h"

class DirectionalLight : public Light {
public:
    
    DirectionalLight(const vec3& direction=vec3(), float intensity=1.0f, const Spectrum& spectrum=Spectrum(1));
    virtual ~DirectionalLight();
    
    void setDirection(const vec3& direction);
    void setIntensity(float intensity);
    void setSpectrum(const Spectrum& spectrum);
    
    virtual Spectrum le(const Ray& ray, const Intersection* intersection=nullptr) const;
    virtual Spectrum sampleL(const vec3& point, float rayEpsilon,
                             const LightSample& lightSample,
                             vec3* wi, VisibilityTester* vt) const;
    
private:
    vec3        _direction;
    float       _intensity;
    Spectrum    _spectrum;
};

#endif /* defined(__CSE168_Rendering__DirectionalLight__) */
