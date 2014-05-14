//
//  Light.h
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/3/14.
//
//

#ifndef __CSE168_Rendering__Light__
#define __CSE168_Rendering__Light__

#include "Core.h"
#include "Sampling.h"
#include "Spectrum.h"
#include "Ray.h"
#include "VisibilityTester.h"
#include "LightSample.h"

class Light {
public:
    Light() : _samplingConfig(1, true, SamplingConfig::UniformDistribution) {};
    virtual ~Light() {};
    
    virtual Spectrum le(const Ray& ray) const = 0;
    virtual Spectrum sampleL(const vec3& point, float rayEpsilon,
                             const LightSample& lightSample,
                             vec3* wi, VisibilityTester* vt) const = 0;
    
    const SamplingConfig& getSamplingConfig() const { return _samplingConfig; }
    void setSamplingConfig(const SamplingConfig& sc) { _samplingConfig = sc; }
    
private:
    SamplingConfig  _samplingConfig;
};

#endif /* defined(__CSE168_Rendering__Light__) */
