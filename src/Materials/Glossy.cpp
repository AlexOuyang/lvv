//
//  Glossy.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/24/14.
//
//

#include "Glossy.h"

std::shared_ptr<Glossy> Glossy::Load(const rapidjson::Value& value) {
    std::shared_ptr<Glossy> material = std::make_shared<Glossy>();
    
    if (value.HasMember("name")) {
        material->setName(value["name"].GetString());
    }
    if (value.HasMember("color")) {
        std::shared_ptr<Texture> texture = Texture::Load(value["color"]);
        if (texture) {
            material->setDiffuseColor(texture);
        }
    }
    if (value.HasMember("indexIn")) {
        material->setIndexIn(value["indexIn"].GetDouble());
    }
    if (value.HasMember("indexOut")) {
        material->setIndexOut(value["indexOut"].GetDouble());
    }
    if (value.HasMember("roughness")) {
        material->setRoughness(value["roughness"].GetDouble());
    }
    
    return material;
}

Glossy::Glossy() : Material(), _color(std::make_shared<UniformVec3Texture>()),
_indexIn(2.33f), _indexOut(1.003f), _roughness(0.2f) {
    
}

Glossy::~Glossy() {
    
}

std::shared_ptr<Material> Glossy::clone() const {
    return std::make_shared<Glossy>(*this);
}

void Glossy::setDiffuseColor(const vec3& color) {
    _color = std::make_shared<UniformVec3Texture>(color);
}

void Glossy::setDiffuseColor(const std::shared_ptr<Texture>& color) {
    _color = color;
}

void Glossy::setIndexIn(float index) {
    _indexIn = index;
}

void Glossy::setIndexOut(float index) {
    _indexOut = index;
}

void Glossy::setRoughness(float roughness) {
    _roughness = roughness;
}

Spectrum Glossy::evaluateBSDF(const vec3 &wo, const vec3 &wi,
                              const Intersection &intersection) const {
    float cosi = glm::abs(glm::dot(wo, intersection.normal));
    vec3 t;
    float fr = refracted(cosi, wo, intersection.normal, _indexOut, _indexIn, &t);
    float cookTorrance = cookTorranceReflection(wo, wi, intersection.normal, _roughness, fr);

    return (1.f - fr) * Spectrum(_color->evaluateVec3(intersection.uv)) + cookTorrance * Spectrum(1.0f);
}

Spectrum Glossy::sampleBSDF(const vec3 &wo, vec3 *wi, const Intersection &intersection,
                            Material::BxDFType type) const {
    if (!(type & BSDFReflection)) {
        return Spectrum(0.0f);
    }
    
    float cosi = glm::abs(glm::dot(wo, intersection.normal));
    vec3 t;
    float fr = refracted(cosi, wo, intersection.normal, _indexOut, _indexIn, &t);
    
    if ((float)rand()/RAND_MAX > fr) {
        *wi = normalize(surfaceToWorld(cosineSampleHemisphere(), intersection));
        return Spectrum(_color->evaluateVec3(intersection.uv));
    } else {
        *wi = reflect(-wo, intersection.normal);
        return Spectrum(1.0f);
    }
    
    *wi = reflect(-wo, intersection.normal);
    return fr;
}