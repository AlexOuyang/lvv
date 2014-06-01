//
//  Matte.h
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/5/14.
//
//

#ifndef __CSE168_Rendering__Matte__
#define __CSE168_Rendering__Matte__

#include "Core/Core.h"
#include "Core/Material.h"
#include "Core/Texture.h"

class Matte : public Material {
public:
    
    static std::shared_ptr<Matte> Load(const rapidjson::Value& value);
    
    Matte(const vec3& color=vec3(1.f));
    Matte(const std::shared_ptr<Texture>& color);
    virtual ~Matte();
    
    virtual std::shared_ptr<Material> clone() const;
    
    virtual void                setDiffuseColor(const vec3& color);
    virtual void                setDiffuseColor(const std::shared_ptr<Texture>& color);
    std::shared_ptr<Texture>    getColor() const;
    
    virtual Spectrum evaluateBSDF(const vec3& wo, const vec3& wi,
                                  const Intersection& intersection) const;
    virtual Spectrum sampleBSDF(const vec3& wo, vec3* wi,
                                const Intersection& intersection,
                                BxDFType type) const;
    virtual BxDFType getBSDFType() const;
    
private:
    std::shared_ptr<Texture>    _color;
};

#endif /* defined(__CSE168_Rendering__Matte__) */
