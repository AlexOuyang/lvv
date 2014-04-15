//
//  MatteMaterial.h
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/5/14.
//
//

#ifndef __CSE168_Rendering__MatteMaterial__
#define __CSE168_Rendering__MatteMaterial__

#include "Core/Material.h"

class MatteMaterial : public Material {
public:
    
    MatteMaterial();
    MatteMaterial(const Spectrum& color);
    virtual ~MatteMaterial();
    
    void setColor(const vec3& color);
    
    virtual Spectrum evaluateBSDF(const vec3& wo, const vec3& wi) const;
    virtual Spectrum sampleBSDF(const vec3& wo, vec3* wi, BxDFType type) const;
    
private:
    Spectrum    _color;
};

#endif /* defined(__CSE168_Rendering__MatteMaterial__) */
