//
//  Glass.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/24/14.
//
//

#include "Glass.h"

Glass::Glass() : Material(),
indexIn(1.0f), indexOut(1.0f),
absorptionColor(vec3(1.0f)), absorptionCoeff(0.0f), roughness(0.2f) {
    
}

Glass::~Glass() {
    
}

Spectrum Glass::evaluateBSDF(const vec3& wo, const vec3 &wi,
                             const Intersection& intersection) const {
    float cosi = glm::abs(glm::dot(wo, intersection.normal));
    vec3 t;
    float fr = refracted(cosi, wo, intersection.normal, indexOut, indexIn, &t);
    float cookTorrance = cookTorranceReflection(wo, wi, intersection.normal, roughness, fr);    
    return cookTorrance * Spectrum(1.0f);
}

Spectrum Glass::sampleBSDF(const vec3 &wo, vec3 *wi, const Intersection &intersection,
                           Material::BxDFType type) const {
    const vec3& n = intersection.normal;
    float cosi = glm::abs(glm::dot(wo, n));
    vec3 t;
    float fr = refracted(cosi, wo, intersection.normal, indexOut, indexIn, &t);
    
    if (type == BSDFAll) {
        float u = (float)rand()/RAND_MAX;
        if (u > fr) {
            *wi = t;
            // If going out of the object, add absorption
            if (glm::dot(wo, n) < 0.0f) {
                return Spectrum(1.0f) * transmittedLight(intersection.t);
            }
            return Spectrum(1.0f);
        } else {
            *wi = glm::reflect(-wo, n);
            return Spectrum(1.0f);
        }
    }
    else if (type & BSDFReflection) {
        *wi = glm::reflect(-wo, n);
        return Spectrum(fr);
    } else if (type & BSDFTransmission) {
        *wi = t;
        return Spectrum(1.0f - fr);
    } else {
        return Spectrum(0.0f);
    }
}

Spectrum Glass::transmittedLight(float distance) const {
    vec3 alpha = (vec3(1.0) - absorptionColor)*absorptionCoeff;
    return Spectrum(glm::exp(-1.0f * alpha * distance));
}