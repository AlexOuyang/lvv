//
//  AreaLight.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/23/14.
//
//

#include "AreaLight.h"

#include <QDebug>

AreaLight::AreaLight(Mesh* mesh) : Light(),
_intensity(1.0f), _spectrum(), _inverseNormal(true) {
    _mesh = mesh;
}

AreaLight::~AreaLight() {
    
}

void AreaLight::setIntensity(float intensity) {
    _intensity = intensity;
}

void AreaLight::setSpectrum(const Spectrum &spectrum) {
    _spectrum = spectrum;
}

const Spectrum& AreaLight::getSpectrum() const {
    return _spectrum;
}

Spectrum AreaLight::le(const Ray& ray) const {
    if (ray.tmax == INFINITY) {
        return Spectrum(0.f);
    }
    return (_spectrum * _intensity);
}

Spectrum AreaLight::sampleL(const vec3 &point, float rayEpsilon,
                            const LightSample& lightSample,
                            vec3 *wi, VisibilityTester *vt) const {
    // Sample random position over mesh
    // Assuming all mesh triangles have the same probability
    const Triangle& triangle = _mesh->triangles[rand() % _mesh->trianglesCount];
    
    // Sample point in triangle
    float alpha = sqrt(lightSample.u)*lightSample.v;
    float beta = 1.0f-sqrt(lightSample.u);
    
    vec3 sampledPosition = (
                            triangle.vertices[0]->position
                            + alpha * (triangle.vertices[1]->position
                                       - triangle.vertices[0]->position)
                            + beta * (triangle.vertices[2]->position
                                         - triangle.vertices[0]->position)
                            );
    
    vec3 normal = triangle.vertices[0]->normal;
    
    if (_inverseNormal) {
        normal *= -1.0f;
    }
    
    vec3 dist = sampledPosition - point;
    float distLength = glm::length(dist);
    dist = dist / distLength;

    *wi = dist;
    
    vt->setSegment(point, rayEpsilon, sampledPosition);
    return (
            (_spectrum * _intensity)
            * dot(dist, normal)
            * (1.0 / (distLength*distLength)));
}