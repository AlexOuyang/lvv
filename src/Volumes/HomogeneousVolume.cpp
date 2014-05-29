//
//  HomogeneousVolume.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 5/21/14.
//
//

#include "HomogeneousVolume.h"

HomogeneousVolume::HomogeneousVolume() : Volume(),
_bounds(), _sigmaA(0.f), _sigmaS(0.f), _le(0.f), _g(0.f) {
    
}

HomogeneousVolume::~HomogeneousVolume() {
    
}

void HomogeneousVolume::setBounds(const AABB& bounds) {
    _bounds = bounds;
}

void HomogeneousVolume::setSigmaA(const Spectrum& sa) {
    _sigmaA = sa;
}

void HomogeneousVolume::setSigmaS(const Spectrum& ss) {
    _sigmaS = ss;
}

void HomogeneousVolume::setLe(const Spectrum& le) {
    _le = le;
}

void HomogeneousVolume::setPhaseParameter(float g) {
    _g = g;
}

AABB HomogeneousVolume::getBoundingBox() const {
    return _bounds;
}

bool HomogeneousVolume::intersectP(const Ray& ray, float* t0, float* t1) const {
    return _bounds.intersectP(ray, t0, t1);
}

Spectrum HomogeneousVolume::sigmaA(const vec3& p) const {
    if (_bounds.intersectP(p)) {
        return _sigmaA;
    }
    return Spectrum(0.f);
}

Spectrum HomogeneousVolume::sigmaS(const vec3& p) const {
    if (_bounds.intersectP(p)) {
        return _sigmaS;
    }
    return Spectrum(0.f);
}

Spectrum HomogeneousVolume::le(const vec3& p) const {
    if (_bounds.intersectP(p)) {
        return _le;
    }
    return Spectrum(0.f);
}

float HomogeneousVolume::phase(const vec3& p, const vec3& wi, const vec3& wo) const {
    if (!_bounds.intersectP(p)) {
        return 0.f;
    }
    return Volume::PhaseHenyeyGreenstein(wi, wo, _g);
}

Spectrum HomogeneousVolume::sigmaT(const vec3& p) const {
    if (_bounds.intersectP(p)) {
        return (_sigmaA + _sigmaS);
    }
    return Spectrum(0.f);
}

Spectrum HomogeneousVolume::tau(const Ray& ray) const {
    float t0, t1;
    
    if (!_bounds.intersectP(ray, &t0, &t1)) {
        return Spectrum(0.f);
    }
    return glm::distance(ray(t0), ray(t1)) * (_sigmaA + _sigmaS);
}