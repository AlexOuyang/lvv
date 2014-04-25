//
//  Glossy.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/24/14.
//
//

#include "Glossy.h"

Glossy::Glossy() : Material() {
    
}

Glossy::~Glossy() {
    
}

Spectrum Glossy::evaluateBSDF(const vec3 &wo, const vec3 &wi,
                              const Intersection &intersection) const {
    float cosi = glm::abs(glm::dot(wo, intersection.normal));
    vec3 t;
    float fr = refracted(cosi, wo, intersection.normal, indexOut, indexIn, &t);
    float cookTorrance = cookTorranceReflection(wo, wi, intersection.normal, roughness, fr);
    return (1.f - fr) * color + (cookTorrance*0.2f) * Spectrum(1.0f);
}

Spectrum Glossy::sampleBSDF(const vec3 &wo, vec3 *wi, const Intersection &intersection,
                            Material::BxDFType type) const {
    if (!(type & BSDFReflection)) {
        return Spectrum(0.0f);
    }
    
    float cosi = glm::abs(glm::dot(wo, intersection.normal));
    vec3 t;
    float fr = refracted(cosi, wo, intersection.normal, indexOut, indexIn, &t);
    *wi = reflect(-wo, intersection.normal);
    return fr;
}