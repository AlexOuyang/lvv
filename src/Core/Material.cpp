//
//  Material.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/23/14.
//
//

#include "Material.h"

Spectrum Material::transmittedLight(float) const {
    return Spectrum(0.0f);
}

float Material::fresnelDielectric(float cosi, float cost, float etai, float etat) {
    float rpar = (etat*cosi - etai*cost)/(etat*cosi + etai*cost);
    float rperp = (etai*cosi - etat*cost)/(etai*cosi + etat*cost);
    
    return 0.5f * (rpar*rpar + rperp*rperp);
}

float Material::fresnelConductor(float cosi, float eta, float k) {
    float e2k2 = (eta*eta + k*k);
    float e2k2cosi = e2k2 * cosi*cosi;
    float rpar2 = (e2k2cosi - 2.0f*eta*cosi + 1.0f) / (e2k2cosi + 2.0f*eta*cosi + 1.0f);
    float rperp2 = (e2k2 - 2.0f*eta*cosi + cosi*cosi) / (e2k2 + 2.0f*eta*cosi + cosi*cosi);
    
    return 0.5f * (rpar2 + rperp2);
}

float Material::cookTorranceReflection(const vec3& wo, const vec3& wi, const vec3& n,
                                       float roughness, float fresnelTerm) {
    vec3 h = glm::normalize(wo + wi);
    // Clamp nh between 0 and 1 (because of float imprecision, higher values may arrise
    // and cause acos being nan)
    float nh = glm::min(1.f, glm::abs(glm::dot(n, h)));
    float vh = glm::abs(glm::dot(wo, h));
    float nv = glm::abs(glm::dot(n, wo));
    float nl = glm::abs(glm::dot(n, wi));
    float m = roughness;
    
    float g = glm::min(1.0f, glm::min((2.f*nh*nv)/(vh), (2.f*nh*nl)/(vh)));
    float d = (1.0f / (m*m * pow(nh, 4.f))) * exp(-((pow(tan(acosf(nh)), 2.f))/(m*m)));
    
    return (fresnelTerm*g*d)/(4.f*M_PI*nl*nv);
}

float Material::refracted(float cosi, const vec3 &wo, vec3 n,
                          float etai, float etat, vec3 *t) {
    // Swap indices and normal if we are exiting the object
    if (glm::dot(wo, n) < 0.0f) {
        std::swap(etai, etat);
        n = -n;
    }
    
    // Fresnel coeff
    float fr;
    
    // Compute transmitted direction
    vec3 z = (etai/etat) * ((-wo) - glm::dot(-wo, n)*n);
    float z2 = glm::dot(z, z);
    
    // Handle total internal reflection
    if (z2 > 1.0f) {
        fr = 1.0f;
    } else {
        *t = z - sqrt(1.0f - z2)*n;
        fr = fresnelDielectric(cosi, glm::dot(*t, -n), etai, etat);
    }
    return fr;
}