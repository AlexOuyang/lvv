//
//  SkyLight.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/3/14.
//
//

#include "SkyLight.h"

SkyLight::SkyLight(const vec3& color) :
Light(), _color(std::make_shared<UniformVec3Texture>(color)), _transform() {
    
}

SkyLight::SkyLight(const std::shared_ptr<Texture>& color) : Light(), _color() {
    if (color) {
        _color = color;
    } else {
        _color = std::make_shared<UniformVec3Texture>(vec3(1.f));
    }
}

SkyLight::~SkyLight() {
    
}

void SkyLight::setColor(const vec3& color) {
    _color = std::make_shared<UniformVec3Texture>(color);
}

void SkyLight::setColor(const std::shared_ptr<Texture> &color) {
    _color = color;
}

void SkyLight::setTransform(const Transform &t) {
    _transform = t;
}

Spectrum SkyLight::le(const Ray & ray, const Intersection*) const {
    vec3 d = normalize(ray.direction);
    d = _transform.applyToVector(d);
    vec2 uv;
    uv.s = ((atan2f(d.x, d.z) + M_PI)
            / (2.f * M_PI));
    uv.t = ((asin(d.y) + 0.5f*M_PI) / M_PI);
    return Spectrum(_color->evaluateVec3(uv));
}

Spectrum SkyLight::sampleL(const vec3&, float, const LightSample&, vec3*, VisibilityTester*) const {
    return Spectrum(0);
}