//
//  AreaLight.h
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/23/14.
//
//

#ifndef __CSE168_Rendering__AreaLight__
#define __CSE168_Rendering__AreaLight__

#include "Core/Light.h"
#include "Shapes/Mesh.h"

class AreaLight : public Light {
public:
    
    AreaLight(Mesh* mesh);
    virtual ~AreaLight();
    
    void setIntensity(float intensity);
    void setSpectrum(const Spectrum& spectrum);
    
    const Spectrum& getSpectrum() const;
    
    virtual Spectrum le(const Ray& ray) const;
    virtual Spectrum sampleL(const vec3& point, float rayEpsilon,
                             const LightSample& lightSample,
                             vec3* wi, VisibilityTester* vt) const;
    
private:
    Mesh*       _mesh;
    float       _intensity;
    Spectrum    _spectrum;
    bool        _inverseNormal;
};

#endif /* defined(__CSE168_Rendering__AreaLight__) */
