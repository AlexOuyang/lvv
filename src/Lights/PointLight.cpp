//
//  PointLight.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/5/14.
//
//

#include "PointLight.h"

PointLight::PointLight(const vec3& position, float intensity, const Spectrum& spectrum)
: Light(), _position(position), _intensity(intensity), _spectrum(spectrum), _noDecay(false) {
    
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

void PointLight::setNoDecay(bool noDecay) {
    _noDecay = noDecay;
}

Spectrum PointLight::le(const Ray&, const Intersection*) const {
    return Spectrum(0.0f);
}

Spectrum PointLight::sampleL(const vec3& point, float rayEpsilon,
                             const LightSample&,
                             vec3* wi, VisibilityTester* vt) const {
    vec3 dist = _position - point;
    *wi = vec3(normalize(dist));
    vt->setSegment(point, rayEpsilon, _position);
    float decay = 1.f;
    if (!_noDecay) {
        decay = (1.0f / dot(dist, dist));
    }
    return (_spectrum * _intensity * decay);
}

Spectrum PointLight::samplePhoton(vec3 *p, vec3 *direction) const {
    *p = _position;
    
    // Sample direction
    float s = (float)rand()/RAND_MAX;
    float t = (float)rand()/RAND_MAX;
    float u = 2.f*M_PI*s;
    float v = sqrt(t*(1-t));
    direction->x = 2.f*v*cos(u);
    direction->y = 1.f - 2.f*t;
    direction->z = 2.f*v*sin(u);
    
    return _spectrum * _intensity * 2.f*M_PI;
}