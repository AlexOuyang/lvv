//
//  Glass.h
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/24/14.
//
//

#ifndef __CSE168_Rendering__Glass__
#define __CSE168_Rendering__Glass__

#include "Core/Material.h"

class Glass : public Material {
public:
    
    static std::shared_ptr<Glass> Load(const rapidjson::Value& value);
    
    Glass();
    ~Glass();
    
    virtual std::shared_ptr<Material> clone() const;

    void setIndexIn(float index);
    void setIndexOut(float index);
    void setAbsorptionColor(const vec3& color);
    void setAbsorptionCoeff(float coeff);
    void setRoughness(float roughness);
    
    virtual Spectrum evaluateBSDF(const vec3& wo, const vec3& wi,
                                  const Intersection& intersection) const;
    virtual Spectrum sampleBSDF(const vec3& wo, vec3* wi,
                                const Intersection& intersection,
                                BxDFType type) const;
    virtual BxDFType getBSDFType() const;
    
    virtual Spectrum transmittedLight(float distance) const;

private:
    float   _indexIn;
    float   _indexOut;
    vec3    _absorptionColor;
    float   _absorptionCoeff;
    float   _roughness;
};

#endif /* defined(__CSE168_Rendering__Glass__) */
