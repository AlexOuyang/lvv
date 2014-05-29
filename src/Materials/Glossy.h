//
//  Glossy.h
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/24/14.
//
//

#ifndef __CSE168_Rendering__Glossy__
#define __CSE168_Rendering__Glossy__

#include "Core/Core.h"
#include "Core/Material.h"
#include "Core/Texture.h"

class Glossy : public Material {
public:
    
    static std::shared_ptr<Glossy> Load(const rapidjson::Value& value);
    
    Glossy();
    ~Glossy();
    
    virtual std::shared_ptr<Material> clone() const;    
    
    virtual Spectrum evaluateBSDF(const vec3& wo, const vec3& wi,
                                  const Intersection& intersection) const;
    virtual Spectrum sampleBSDF(const vec3& wo, vec3* wi, const Intersection& intersection,
                                BxDFType type) const;
    
    virtual void setDiffuseColor(const vec3& color);
    virtual void setDiffuseColor(const std::shared_ptr<Texture>& color);
    
    void setIndexIn(float index);
    void setIndexOut(float index);
    void setRoughness(float roughness);
    
private:
    std::shared_ptr<Texture>    _color;
    float                       _indexIn;
    float                       _indexOut;
    float                       _roughness;
};

#endif /* defined(__CSE168_Rendering__Glossy__) */
