//
//  Metal.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/23/14.
//
//

#include "Metal.h"

Metal::Metal() : Material(), _eta(2.485f), _k(3.433f), _roughness(0.2f), _color() {
    
}

Metal::~Metal() {
    
}

void Metal::setIndices(float eta, float k) {
    _eta = eta;
    _k = k;
}

void Metal::setRoughness(float roughness) {
    _roughness = roughness;
}

void Metal::setColor(const vec3& color) {
    _color = color;
}

Spectrum Metal::evaluateBSDF(const vec3& wo, const vec3& wi,
                             const Intersection& intersection) const {
    float cosi = glm::abs(glm::dot(wo, intersection.normal));
    float f = fresnelConductor(cosi, _eta, _k);
    float cookTorrance = cookTorranceReflection(wo, wi, intersection.normal, _roughness, f);
    return Spectrum(_color) * cookTorrance;
}

Spectrum Metal::sampleBSDF(const vec3& wo, vec3* wi, const Intersection& intersection,
                           BxDFType type) const {
    if (!(type & BSDFReflection)) {
        return Spectrum(0.0f);
    }
    *wi = glm::reflect(-wo, intersection.normal);
    float cosi = glm::abs(glm::dot(wo, intersection.normal));
    return _color * fresnelConductor(cosi, _eta, _k);
}