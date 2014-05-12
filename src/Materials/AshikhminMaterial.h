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
#include "Core/Texture.h"

class AshikhminMaterial : public Material {
public:
    
    AshikhminMaterial();
    virtual ~AshikhminMaterial();
    
    void setDiffuseColor(Texture* t);
    void setSpecularColor(Texture* t);
    void setDiffuseIntensity(Texture* t);
    void setSpecularIntensity(Texture* t);
    void setRoughness(Texture* tu, Texture* tv);
    
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
    Texture*    _diffuseColor;
    Texture*    _specularColor;
    Texture*    _diffuseIntensity;
    Texture*    _specularIntensity;
    Texture*    _roughnessU;
    Texture*    _roughnessV;
};

#endif /* defined(__CSE168_Rendering__AshikhminMaterial__) */
