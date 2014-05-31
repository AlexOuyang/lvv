//
//  DensityVolume.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 5/24/14.
//
//

#include "DensityVolume.h"

#include "Core/Ray.h"

DensityVolume::DensityVolume() : Volume(), _bounds(), _sigmaA(0.f), _sigmaS(0.f), _le(0.f), _g(0.f) {
    
}

DensityVolume::~DensityVolume() {
    
}

void DensityVolume::setTransform(const Transform& objectToWorld) {
    _worldToObject = Transform::Inverse(objectToWorld);
}

void DensityVolume::setBounds(const AABB& bounds) {
    _bounds = bounds;
}

void DensityVolume::setSigmaA(const Spectrum& sa) {
    _sigmaA = sa;
}

void DensityVolume::setSigmaS(const Spectrum& ss) {
    _sigmaS = ss;
}

void DensityVolume::setLe(const Spectrum& le) {
    _le = le;
}

void DensityVolume::setPhaseParameter(float g) {
    _g = g;
}

AABB DensityVolume::getBoundingBox() const {
    return AABB();
}

bool DensityVolume::intersectP(const Ray &ray, float *t0, float *t1) const {
    return _bounds.intersectP(ray, t0, t1);
}

Spectrum DensityVolume::sigmaA(const vec3 &p) const {
    return density(p) * _sigmaA;
}

Spectrum DensityVolume::sigmaS(const vec3 &p) const {
    return density(p) * _sigmaS;
}

Spectrum DensityVolume::le(const vec3 &p) const {
    return density(p) * _le;
}

float DensityVolume::phase(const vec3& p, const vec3& wi, const vec3& wo) const {
    if (!_bounds.intersectP(p)) {
        return 0.f;
    }
    return Volume::PhaseHenyeyGreenstein(wi, wo, _g);
}

Spectrum DensityVolume::sigmaT(const vec3& p) const {
    return density(p) * (_sigmaA + _sigmaS);
}

Spectrum DensityVolume::tau(const Ray& ray) const {
    float t0, t1;
    if (!_bounds.intersectP(ray, &t0, &t1)) {
        return Spectrum(0.f);
    }
    
    const float stepSize = 0.1f;
    
    Spectrum tau = 0.f;
    
    // Add sampling offset
    t0 += ((float)rand()/RAND_MAX)*stepSize;
    
    while (t0 < t1) {
        tau += sigmaT(ray(t0)) * stepSize;
        t0 = t0 + stepSize;
    }
    
    return tau;
}