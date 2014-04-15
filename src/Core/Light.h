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
#include "Spectrum.h"
#include "Ray.h"
#include "VisibilityTester.h"

class Light {
public:    
    virtual ~Light() {};
    
    virtual Spectrum le(const Ray& ray) const = 0;
    virtual Spectrum sampleL(const vec3& point, float rayEpsilon,
                             vec3* wi, VisibilityTester* vt) const = 0;
};

#endif /* defined(__CSE168_Rendering__Light__) */
