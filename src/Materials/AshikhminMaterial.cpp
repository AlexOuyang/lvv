//
//  AshikhminMaterial.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 5/7/14.
//
//

#include "AshikhminMaterial.h"

std::shared_ptr<AshikhminMaterial> AshikhminMaterial::Load(const rapidjson::Value& value) {
    std::shared_ptr<AshikhminMaterial> material = std::make_shared<AshikhminMaterial>();
    
    if (value.HasMember("name")) {
        material->setName(value["name"].GetString());
    }
    if (value.HasMember("diffuseColor")) {
        std::shared_ptr<Texture> texture = Texture::Load(value["diffuseColor"]);
        if (texture) {
            material->setDiffuseColor(texture);
        }
    }
    if (value.HasMember("specularColor")) {
        std::shared_ptr<Texture> texture = Texture::Load(value["specularColor"]);
        if (texture) {
            material->setSpecularColor(texture);
        }
    }
    if (value.HasMember("diffuseIntensity")) {
        const rapidjson::Value& intensityValue = value["diffuseIntensity"];
        if (intensityValue.IsNumber()) {
            material->setDiffuseIntensity(intensityValue.GetDouble());
        } else {
            std::shared_ptr<Texture> texture = Texture::Load(intensityValue);
            if (texture) {
                material->setDiffuseIntensity(texture);
            }
        }
    }
    if (value.HasMember("specularIntensity")) {
        const rapidjson::Value& intensityValue = value["specularIntensity"];
        if (intensityValue.IsNumber()) {
            material->setSpecularIntensity(intensityValue.GetDouble());
        } else {
            std::shared_ptr<Texture> texture = Texture::Load(intensityValue);
            if (texture) {
                material->setSpecularIntensity(texture);
            }
        }
    }
    if (value.HasMember("roughnessU")) {
        const rapidjson::Value& roughnessValue = value["roughnessU"];
        if (roughnessValue.IsNumber()) {
            material->setRoughnessU(roughnessValue.GetDouble());
        } else {
            std::shared_ptr<Texture> texture = Texture::Load(roughnessValue);
            if (texture) {
                material->setRoughnessU(texture);
            }
        }
    }
    if (value.HasMember("roughnessV")) {
        const rapidjson::Value& roughnessValue = value["roughnessV"];
        if (roughnessValue.IsNumber()) {
            material->setRoughnessV(roughnessValue.GetDouble());
        } else {
            std::shared_ptr<Texture> texture = Texture::Load(roughnessValue);
            if (texture) {
                material->setRoughnessV(texture);
            }
        }
    }
    if (value.HasMember("roughness")) {
        const rapidjson::Value& roughnessValue = value["roughness"];
        if (roughnessValue.IsArray() && roughnessValue.Size() == 2) {
            material->setRoughness(roughnessValue[0u].GetDouble(), roughnessValue[1u].GetDouble());
        }
    }
    
    return material;
}

AshikhminMaterial::AshikhminMaterial() :
Material(),
_diffuseColor(std::make_shared<UniformVec3Texture>(vec3(1.f))),
_specularColor(std::make_shared<UniformVec3Texture>(vec3(0.f))),
_diffuseIntensity(std::make_shared<UniformFloatTexture>(1.f)),
_specularIntensity(std::make_shared<UniformFloatTexture>(0.f)),
_roughnessU(std::make_shared<UniformFloatTexture>(1000.f)),
_roughnessV(new UniformFloatTexture(1000.f)) {
    
}

AshikhminMaterial::~AshikhminMaterial() {
    
}

std::shared_ptr<Material> AshikhminMaterial::clone() const {
    return std::make_shared<AshikhminMaterial>(*this);
}

void AshikhminMaterial::setDiffuseColor(const std::shared_ptr<Texture>& t) {
    _diffuseColor = t;
}

void AshikhminMaterial::setSpecularColor(const std::shared_ptr<Texture>& t) {
    _specularColor = t;
}

void AshikhminMaterial::setDiffuseIntensity(const std::shared_ptr<Texture>& t) {
    _diffuseIntensity = t;
}

void AshikhminMaterial::setSpecularIntensity(const std::shared_ptr<Texture>& t) {
    _specularIntensity = t;
}

void AshikhminMaterial::setRoughnessU(const std::shared_ptr<Texture>& tu) {
    _roughnessU = tu;
}

void AshikhminMaterial::setRoughnessV(const std::shared_ptr<Texture>& tv) {
    _roughnessV = tv;
}

void AshikhminMaterial::setRoughness(const std::shared_ptr<Texture>& tu, const std::shared_ptr<Texture>& tv) {
    if (tu) {
        _roughnessU = tu;
    }
    if (tv) {
        _roughnessV = tv;
    }
}

void AshikhminMaterial::setDiffuseColor(const vec3& color) {
    _diffuseColor = std::make_shared<UniformVec3Texture>(color);
}

void AshikhminMaterial::setSpecularColor(const vec3& color) {
    _specularColor = std::make_shared<UniformVec3Texture>(color);
}

void AshikhminMaterial::setDiffuseIntensity(float intensity) {
    _diffuseIntensity = std::make_shared<UniformFloatTexture>(intensity);
}

void AshikhminMaterial::setSpecularIntensity(float intensity) {
    _specularIntensity = std::make_shared<UniformFloatTexture>(intensity);
}

void AshikhminMaterial::setRoughnessU(float n) {
    _roughnessU = std::make_shared<UniformFloatTexture>(n);
}

void AshikhminMaterial::setRoughnessV(float n) {
    _roughnessV = std::make_shared<UniformFloatTexture>(n);
}

void AshikhminMaterial::setRoughness(float nu, float nv) {
    _roughnessU = std::make_shared<UniformFloatTexture>(nu);
    _roughnessV = std::make_shared<UniformFloatTexture>(nv);
}

Spectrum AshikhminMaterial::evaluateBSDF(const vec3& wo, const vec3& wi,
                             const Intersection& intersection) const {
    vec3 n = intersection.normal;
    if (dot(wo, n) < 0) {
        n = -n;
    }
    
    float nu = _roughnessU->evaluateFloat(intersection.uv);
    float nv = _roughnessV->evaluateFloat(intersection.uv);
    float diffuseIntensity = _diffuseIntensity->evaluateFloat(intersection.uv);
    float specularIntensity = _specularIntensity->evaluateFloat(intersection.uv);
    vec3 diffuseColor = _diffuseColor->evaluateVec3(intersection.uv);
    vec3 specularColor = _specularColor->evaluateVec3(intersection.uv);
    
    vec3 h = normalize(wo + wi);
    float hu = dot(h, intersection.tangentU);
    float hv = dot(h, intersection.tangentV);
    float nh = dot(h, n);
    float hk = dot(h, wo);
    
    float specularComponent = sqrt((nu + 1.f)*(nv + 1.f)) / (8.f*M_PI);
    float exponent = (nu*hu*hu +nv*hv*hv) / (1.f - nh*nh);
    specularComponent *= pow(nh, exponent) / (hk * max(dot(n, wi), dot(n, wo)));
    specularComponent *= specularIntensity + (1.f - specularIntensity)*pow(1.f - hk, 5.f);
    
    float diffuseComponent = (28.f*diffuseIntensity) / (23.f*M_PI);
    diffuseComponent *= (1.f - specularIntensity);
    diffuseComponent *= (1.f - pow(1.f - (dot(n, wi)/2.f), 5.f));
    diffuseComponent *= (1.f - pow(1.f - (dot(n, wo)/2.f), 5.f));
    
    // Clamp specular component to avoid ridiculously high values
    specularComponent = min(42.f, specularComponent);
    
    return Spectrum(diffuseColor*diffuseComponent + specularColor*specularComponent);
}

Spectrum AshikhminMaterial::sampleBSDF(const vec3& wo, vec3* wi, const Intersection& intersection,
                                       BxDFType) const {
    float nu = _roughnessU->evaluateFloat(intersection.uv);
    float nv = _roughnessV->evaluateFloat(intersection.uv);
    float specularIntensity = _specularIntensity->evaluateFloat(intersection.uv);
    vec3 diffuseColor = _diffuseColor->evaluateVec3(intersection.uv);
    vec3 specularColor = _specularColor->evaluateVec3(intersection.uv);
    
    if ((float)rand()/RAND_MAX < specularIntensity) {
        float s1 = (float)rand()/RAND_MAX;
        float s2 = (float)rand()/RAND_MAX;
        
        float phi = atanf(sqrt((nu+1.f)/(nv+1.f))*tan((M_PI*s1)/2.f));
        
        float quadrant = (float)rand()/RAND_MAX;
        
        if (quadrant >= 0.25f && quadrant < 0.5f) {
            phi = M_PI - phi;
        } else if (quadrant >= 0.5f && quadrant < 0.75f) {
            phi += M_PI;
        } else if (quadrant >= 0.75f) {
            phi = 2.f*M_PI - phi;
        }
        
        float cosphi = cos(phi);
        float sinphi = sin(phi);
        float exponent = 1.f / (nu * cosphi*cosphi + nv * sinphi*sinphi + 1.f);
        float theta = acosf(powf(1.f - s2, exponent));
        
        vec3 h = vec3(
                      cos(phi)*sin(theta),
                      cos(theta),
                      sin(phi)*sin(theta)
                      );
        h = normalize(surfaceToWorld(h, intersection));
        *wi = normalize(-wo + 2.f * (dot(wo, h)*h));
        if (dot(intersection.normal, *wi) < 0.f) {
            return Spectrum(0.f);
        }
        return specularColor;
    } else {
        *wi = normalize(surfaceToWorld(cosineSampleHemisphere(), intersection));
        return diffuseColor;
    }
}