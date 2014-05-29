//
//  Metal.h
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/23/14.
//
//

#ifndef __CSE168_Rendering__Metal__
#define __CSE168_Rendering__Metal__

#include "Core/Core.h"
#include "Core/Material.h"
#include "Core/Texture.h"

class Metal : public Material {
public:
    
    static std::shared_ptr<Metal> Load(const rapidjson::Value& value);
    
    Metal();
    ~Metal();
    
    virtual std::shared_ptr<Material> clone() const;
    
    virtual Spectrum evaluateBSDF(const vec3& wo, const vec3& wi,
                                  const Intersection& intersection) const;
    virtual Spectrum sampleBSDF(const vec3& wo, vec3* wi, const Intersection& intersection,
                                BxDFType type) const;
    
    void setEta(float eta);
    void setK(float k);
    void setIndices(float eta, float k);
    void setRoughness(float roughness);
    
    virtual void setDiffuseColor(const vec3& color);
    virtual void setDiffuseColor(const std::shared_ptr<Texture>& color);
    
private:
    float                       _eta;
    float                       _k;
    float                       _roughness;
    std::shared_ptr<Texture>    _color;
};

#endif /* defined(__CSE168_Rendering__Metal__) */
