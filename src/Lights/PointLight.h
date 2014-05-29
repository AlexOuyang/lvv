//
//  PointLight.h
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/5/14.
//
//

#ifndef __CSE168_Rendering__vec3Light__
#define __CSE168_Rendering__vec3Light__

#include "Core/Core.h"
#include "Core/Light.h"

class PointLight : public Light {
public:
    
    PointLight(const vec3& position=vec3(), float intensity=1.0f, const Spectrum& spectrum=Spectrum(1));
    virtual ~PointLight();
    
    void setPosition(const vec3& position);
    void setIntensity(float intensity);
    void setSpectrum(const Spectrum& spectrum);
    
    virtual Spectrum le(const Ray& ray, const Intersection* intersection=nullptr) const;
    virtual Spectrum sampleL(const vec3& point, float rayEpsilon,
                             const LightSample& lightSample,
                             vec3* wi, VisibilityTester* vt) const;
    
private:    
    vec3        _position;
    float       _intensity;
    Spectrum    _spectrum;
};

#endif /* defined(__CSE168_Rendering__vec3Light__) */
