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
#include "Core/Texture.h"

class Matte : public Material {
public:
    
    Matte(const vec3& color=vec3(1.f));
    Matte(Texture* color);
    virtual ~Matte();
    
    void        setColor(const vec3& color);
    Texture*    getColor() const;
    
    virtual Spectrum evaluateBSDF(const vec3& wo, const vec3& wi,
                                  const Intersection& intersection) const;
    virtual Spectrum sampleBSDF(const vec3& wo, vec3* wi, const Intersection& intersection,
                                BxDFType type) const;
    
private:
    Texture*    _color;
};

#endif /* defined(__CSE168_Rendering__Matte__) */
