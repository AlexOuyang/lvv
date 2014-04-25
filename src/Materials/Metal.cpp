//
//  Metal.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/23/14.
//
//

#include "Metal.h"

Metal::Metal() : Material() {
    
}

Metal::~Metal() {
    
}

Spectrum Metal::evaluateBSDF(const vec3& wo, const vec3& wi,
                             const Intersection& intersection) const {
    float cosi = glm::abs(glm::dot(wo, intersection.normal));
    float f = fresnelConductor(cosi, eta, k);
    float cookTorrance = cookTorranceReflection(wo, wi, intersection.normal, roughness, f);
    return color * (cookTorrance*0.2f);
}

Spectrum Metal::sampleBSDF(const vec3& wo, vec3* wi, const Intersection& intersection,
                           BxDFType type) const {
    if (!(type & BSDFReflection)) {
        return Spectrum(0.0f);
    }
    *wi = glm::reflect(-wo, intersection.normal);
    float cosi = glm::abs(glm::dot(wo, intersection.normal));
    return color * fresnelConductor(cosi, eta, k);
}