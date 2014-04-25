//
//  Matte.h
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/5/14.
//
//

#ifndef __CSE168_Rendering__Matte__
#define __CSE168_Rendering__Matte__

#include "Core/Material.h"

class Matte : public Material {
public:
    
    Matte();
    Matte(const Spectrum& color);
    virtual ~Matte();
    
    void setColor(const vec3& color);
    
    virtual Spectrum evaluateBSDF(const vec3& wo, const vec3& wi,
                                  const Intersection& intersection) const;
    virtual Spectrum sampleBSDF(const vec3& wo, vec3* wi, const Intersection& intersection,
                                BxDFType type) const;
    
private:
    Spectrum    _color;
};

#endif /* defined(__CSE168_Rendering__Matte__) */
