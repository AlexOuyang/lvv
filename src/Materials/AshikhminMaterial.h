//
//  AshikhminMaterial.h
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 5/7/14.
//
//

#ifndef __CSE168_Rendering__AshikhminMaterial__
#define __CSE168_Rendering__AshikhminMaterial__

#include "Core/Core.h"
#include "Core/Material.h"
#include "Core/Texture.h"

class AshikhminMaterial : public Material {
public:
    
    static std::shared_ptr<AshikhminMaterial> Load(const rapidjson::Value& value);
    
    AshikhminMaterial();
    virtual ~AshikhminMaterial();
    
    virtual std::shared_ptr<Material> clone() const;    
    
    virtual void setDiffuseColor(const std::shared_ptr<Texture>& t);
    void setSpecularColor(const std::shared_ptr<Texture>& t);
    void setDiffuseIntensity(const std::shared_ptr<Texture>& t);
    void setSpecularIntensity(const std::shared_ptr<Texture>& t);
    
    void setRoughnessU(const std::shared_ptr<Texture>& tu);
    void setRoughnessV(const std::shared_ptr<Texture>& tv);
    void setRoughness(const std::shared_ptr<Texture>& tu,
                      const std::shared_ptr<Texture>& tv);
    
    virtual void setDiffuseColor(const vec3& color);
    void setSpecularColor(const vec3& color);
    void setDiffuseIntensity(float intensity);
    void setSpecularIntensity(float intensity);
    void setRoughnessU(float n);
    void setRoughnessV(float n);
    void setRoughness(float nu, float nv);
    
    virtual Spectrum evaluateBSDF(const vec3& wo, const vec3& wi,
                                  const Intersection& intersection) const;
    virtual Spectrum sampleBSDF(const vec3& wo, vec3* wi,
                                const Intersection& intersection,
                                BxDFType type, BxDFType* sampledType) const;
    
private:
    std::shared_ptr<Texture>    _diffuseColor;
    std::shared_ptr<Texture>    _specularColor;
    std::shared_ptr<Texture>    _diffuseIntensity;
    std::shared_ptr<Texture>    _specularIntensity;
    std::shared_ptr<Texture>    _roughnessU;
    std::shared_ptr<Texture>    _roughnessV;
};

#endif /* defined(__CSE168_Rendering__AshikhminMaterial__) */
