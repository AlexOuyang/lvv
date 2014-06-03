//
//  Material.h
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/5/14.
//
//

#ifndef CSE168_Rendering_Material_h
#define CSE168_Rendering_Material_h

#include "Core.h"
#include "Spectrum.h"
#include "Intersection.h"
#include "Texture.h"

class Material {
public:
    
    enum BxDFType {
        BSDFReflection      = 1 << 0,
        BSDFTransmission    = 1 << 1,
        BSDFDiffuse         = 1 << 2,
        BSDFAll             = BSDFReflection | BSDFTransmission | BSDFDiffuse
    };
    
    static std::shared_ptr<Material> Load(const rapidjson::Value& value);
    
    virtual ~Material();
    
    virtual std::shared_ptr<Material> clone() const = 0;
    
    const std::string& getName() const;
    void setName(const std::string& name);
    
    virtual void setDiffuseColor(const vec3& color);
    virtual void setDiffuseColor(const std::shared_ptr<Texture>& color);
    
    virtual Spectrum transmittedLight(float distance) const;
    
    virtual Spectrum evaluateBSDF(const vec3& wo, const vec3& wi,
                                  const Intersection& intersection) const = 0;
    virtual Spectrum sampleBSDF(const vec3& wo, vec3* wi,
                                const Intersection& intersection,
                                BxDFType type, BxDFType* sampledType) const = 0;
    virtual BxDFType getBSDFType() const { return (BxDFType)0; };
    
    // Utility functions for computing material bsdf's
    static float fresnelDielectric(float cosi, float cost, float etai, float etat);
    static float fresnelConductor(float cosi, float eta, float k);
    static float cookTorranceReflection(const vec3& wo, const vec3& wi, const vec3& n,
                                        float roughness, float fresnelTerm);
    
    static float refracted(float cosi, const vec3& wo, vec3 n,
                           float etai, float etat, vec3* t);
    
    // Sampling functions
    static vec3 cosineSampleHemisphere();
    
    static vec3 surfaceToWorld(const vec3& v, const Intersection& intersection);
    
private:
    std::string _name;
};

#endif
