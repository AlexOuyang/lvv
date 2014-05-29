//
//  Matte.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/5/14.
//
//

#include "Matte.h"

std::shared_ptr<Matte> Matte::Load(const rapidjson::Value& value) {
    std::shared_ptr<Matte> material = std::make_shared<Matte>();
    
    if (value.HasMember("name")) {
        material->setName(value["name"].GetString());
    }
    if (value.HasMember("color")) {
        std::shared_ptr<Texture> texture = Texture::Load(value["color"]);
        if (texture) {
            material->setDiffuseColor(texture);
        }
    }
    
    return material;
}

Matte::Matte(const vec3& color) : _color(std::make_shared<UniformVec3Texture>(color)) {

}

Matte::Matte(const std::shared_ptr<Texture>& color) : _color() {
    if (color) {
        _color = color;
    } else {
        _color = std::make_shared<UniformVec3Texture>(vec3(1.f));
    }
}

Matte::~Matte() {
}

std::shared_ptr<Material> Matte::clone() const {
    return std::make_shared<Matte>(*this);
}

void Matte::setDiffuseColor(const vec3& color) {
    _color = std::make_shared<UniformVec3Texture>(color);
}

void Matte::setDiffuseColor(const std::shared_ptr<Texture>& color) {
    _color = color;
}

std::shared_ptr<Texture> Matte::getColor() const {
    return _color;
}

Spectrum Matte::evaluateBSDF(const vec3&, const vec3&,
                             const Intersection& intersection) const {
    return Spectrum(_color->evaluateVec3(intersection.uv) / (float)M_PI);
}

Spectrum Matte::sampleBSDF(const vec3&, vec3* wi, const Intersection& intersection,
                           BxDFType type) const {
    if (!(type & BSDFDiffuse)) {
        return Spectrum(0.0f);
    }
    
    *wi = normalize(surfaceToWorld(cosineSampleHemisphere(), intersection));
    return _color->evaluateVec3(intersection.uv);
}