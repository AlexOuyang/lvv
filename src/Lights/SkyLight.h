//
//  SkyLight.h
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/3/14.
//
//

#ifndef __CSE168_Rendering__SkyLight__
#define __CSE168_Rendering__SkyLight__

#include "Core/Light.h"
#include "Core/Spectrum.h"

class SkyLight : public Light {
public:
    
    SkyLight(const Spectrum& spectrum);
    virtual ~SkyLight();
    
    virtual Spectrum le(const Ray& ray) const;
    virtual Spectrum sampleL(const vec3& point, float rayEpsilon,
                             const LightSample& lightSample,
                             vec3* wi, VisibilityTester* vt) const;
    
private:
    Spectrum    _spectrum;
};

#endif /* defined(__CSE168_Rendering__SkyLight__) */
