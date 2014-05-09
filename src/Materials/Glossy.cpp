//
//  Glossy.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/24/14.
//
//

#include "Glossy.h"

Glossy::Glossy() : Material(), color(), indexIn(2.33f), indexOut(1.003f), roughness(0.2f) {
    
}

Glossy::~Glossy() {
    
}

Spectrum Glossy::evaluateBSDF(const vec3 &wo, const vec3 &wi,
                              const Intersection &intersection) const {
    float cosi = glm::abs(glm::dot(wo, intersection.normal));
    vec3 t;
    float fr = refracted(cosi, wo, intersection.normal, indexOut, indexIn, &t);
    float cookTorrance = cookTorranceReflection(wo, wi, intersection.normal, roughness, fr);

    return (1.f - fr) * color + cookTorrance * Spectrum(1.0f);
}

Spectrum Glossy::sampleBSDF(const vec3 &wo, vec3 *wi, const Intersection &intersection,
                            Material::BxDFType type) const {
    if (!(type & BSDFReflection)) {
        return Spectrum(0.0f);
    }
    
    float cosi = glm::abs(glm::dot(wo, intersection.normal));
    vec3 t;
    float fr = refracted(cosi, wo, intersection.normal, indexOut, indexIn, &t);
    
    if ((float)rand()/RAND_MAX > fr) {
        *wi = normalize(surfaceToWorld(cosineSampleHemisphere(), intersection));
        return color;
    } else {
        *wi = reflect(-wo, intersection.normal);
        return Spectrum(1.0f);
    }
    
    *wi = reflect(-wo, intersection.normal);
    return fr;
}