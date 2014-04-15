//
//  DirectionalLight.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/5/14.
//
//

#include "DirectionalLight.h"

DirectionalLight::DirectionalLight(const vec3& direction, float intensity, const Spectrum& spectrum)
: _direction(direction), _intensity(intensity), _spectrum(spectrum) {
    
}

DirectionalLight::~DirectionalLight() {
    
}

void DirectionalLight::setDirection(const vec3 &direction) {
    _direction = normalize(direction);
}

void DirectionalLight::setIntensity(float intensity) {
    _intensity = intensity;
}

void DirectionalLight::setSpectrum(const Spectrum &spectrum) {
    _spectrum = spectrum;
}

Spectrum DirectionalLight::le(const Ray&) const {
    return Spectrum(0);
}

Spectrum DirectionalLight::sampleL(const vec3& point, float rayEpsilon,
                                   vec3* wi, VisibilityTester* vt) const {
    *wi = -_direction;
    vt->setRay(point, rayEpsilon, *wi);
    return _intensity * _spectrum;
}