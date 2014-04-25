//
//  PointLight.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/5/14.
//
//

#include "PointLight.h"

PointLight::PointLight(const vec3& position, float intensity, const Spectrum& spectrum)
: Light(), _position(position), _intensity(intensity), _spectrum(spectrum) {
    
}

PointLight::~PointLight() {
    
}

void PointLight::setPosition(const vec3& position) {
    _position = position;
}

void PointLight::setIntensity(float intensity) {
    _intensity = intensity;
}

void PointLight::setSpectrum(const Spectrum& spectrum) {
    _spectrum = spectrum;
}

Spectrum PointLight::le(const Ray&) const {
    return Spectrum(0.0f);
}

Spectrum PointLight::sampleL(const vec3& point, float rayEpsilon,
                             const LightSample&,
                             vec3* wi, VisibilityTester* vt) const {
    vec3 dist = _position - point;
    *wi = vec3(normalize(dist));
    vt->setSegment(point, rayEpsilon, _position);
    return (_spectrum * _intensity) * (1.0 / dot(dist, dist));
}