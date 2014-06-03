//
//  Metal.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/23/14.
//
//

#include "Metal.h"

std::shared_ptr<Metal> Metal::Load(const rapidjson::Value& value) {
    std::shared_ptr<Metal> material = std::make_shared<Metal>();
    
    if (value.HasMember("name")) {
        material->setName(value["name"].GetString());
    }
    if (value.HasMember("eta")) {
        material->setEta(value["eta"].GetDouble());
    }
    if (value.HasMember("k")) {
        material->setK(value["k"].GetDouble());
    }
    if (value.HasMember("roughness")) {
        material->setRoughness(value["roughness"].GetDouble());
    }
    if (value.HasMember("color")) {
        std::shared_ptr<Texture> texture = Texture::Load(value["color"]);
        if (texture) {
            material->setDiffuseColor(texture);
        }
    }
    
    return material;
}

Metal::Metal() : Material(), _eta(2.485f), _k(3.433f), _roughness(0.2f),
_color(std::make_shared<UniformVec3Texture>()) {
    
}

Metal::~Metal() {
    
}

std::shared_ptr<Material> Metal::clone() const {
    return std::make_shared<Metal>(*this);
}

void Metal::setEta(float eta) {
    _eta = eta;
}

void Metal::setK(float k) {
    _k = k;
}

void Metal::setIndices(float eta, float k) {
    _eta = eta;
    _k = k;
}

void Metal::setRoughness(float roughness) {
    _roughness = roughness;
}

void Metal::setDiffuseColor(const vec3& color) {
    _color = std::make_shared<UniformVec3Texture>(color);
}

void Metal::setDiffuseColor(const std::shared_ptr<Texture>& color) {
    _color = color;
}

Spectrum Metal::evaluateBSDF(const vec3& wo, const vec3& wi,
                             const Intersection& intersection) const {
    vec3 h = normalize(wo + wi);
    float blinn = pow(dot(intersection.normal, h), _roughness);
    return Spectrum(_color->evaluateVec3(intersection.uv)) * blinn * 0.f;
}

Spectrum Metal::sampleBSDF(const vec3& wo, vec3* wi, const Intersection& intersection,
                           BxDFType type, BxDFType* sampledType) const {
    if (!(type & BSDFReflection)) {
        *sampledType = (BxDFType)0;
        return Spectrum(0.0f);
    }
    *sampledType = BSDFReflection;
    *wi = glm::reflect(-wo, intersection.normal);
    float cosi = glm::abs(glm::dot(wo, intersection.normal));
    return Spectrum(_color->evaluateVec3(intersection.uv)) * fresnelConductor(cosi, _eta, _k);
}

Material::BxDFType Metal::getBSDFType() const {
    return BSDFReflection;
}