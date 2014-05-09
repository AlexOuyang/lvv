//
//  AshikhminMaterial.h
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 5/7/14.
//
//

#ifndef __CSE168_Rendering__AshikhminMaterial__
#define __CSE168_Rendering__AshikhminMaterial__

#include "Core/Material.h"

class AshikhminMaterial : public Material {
public:
    
    AshikhminMaterial();
    virtual ~AshikhminMaterial();
    
    void setDiffuseColor(const vec3& color);
    void setSpecularColor(const vec3& color);
    void setDiffuseIntensity(float intensity);
    void setSpecularIntensity(float intensity);
    void setRoughness(float nu, float nv);
    
    virtual Spectrum evaluateBSDF(const vec3& wo, const vec3& wi,
                                  const Intersection& intersection) const;
    virtual Spectrum sampleBSDF(const vec3& wo, vec3* wi, const Intersection& intersection,
                                BxDFType type) const;
    
private:
    Spectrum    _diffuseColor;
    Spectrum    _specularColor;
    float       _diffuseIntensity;
    float       _specularIntensity;
    float       _roughnessU;
    float       _roughnessV;
};

#endif /* defined(__CSE168_Rendering__AshikhminMaterial__) */
