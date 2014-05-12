//
//  SkyLight.h
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/3/14.
//
//

#ifndef __CSE168_Rendering__SkyLight__
#define __CSE168_Rendering__SkyLight__

#include "Core/Core.h"
#include "Core/Light.h"
#include "Core/Spectrum.h"
#include "Core/Texture.h"
#include "Core/Transform.h"

class SkyLight : public Light {
public:
    
    SkyLight(const vec3& color);
    SkyLight(Texture* color);
    virtual ~SkyLight();
    
    void setTransform(const Transform& t);
    
    virtual Spectrum le(const Ray& ray) const;
    virtual Spectrum sampleL(const vec3& point, float rayEpsilon,
                             const LightSample& lightSample,
                             vec3* wi, VisibilityTester* vt) const;
    
private:
    Texture*    _color;
    Transform   _transform;
};

#endif /* defined(__CSE168_Rendering__SkyLight__) */
