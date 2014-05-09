//
//  AshikhminMaterial.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 5/7/14.
//
//

#include "AshikhminMaterial.h"

AshikhminMaterial::AshikhminMaterial() :
Material(),
_diffuseColor(1.f), _specularColor(0.f),
_diffuseIntensity(1.f), _specularIntensity(0.f),
_roughnessU(1000.f), _roughnessV(1000.f) {
    
}

AshikhminMaterial::~AshikhminMaterial() {
    
}

void AshikhminMaterial::setDiffuseColor(const vec3& color) {
    _diffuseColor = Spectrum(color);
}

void AshikhminMaterial::setSpecularColor(const vec3& color) {
    _specularColor = Spectrum(color);
}

void AshikhminMaterial::setDiffuseIntensity(float intensity) {
    _diffuseIntensity = intensity;
}

void AshikhminMaterial::setSpecularIntensity(float intensity) {
    _specularIntensity = intensity;
}

void AshikhminMaterial::setRoughness(float nu, float nv) {
    _roughnessU = nu;
    _roughnessV = nv;
}

Spectrum AshikhminMaterial::evaluateBSDF(const vec3& wo, const vec3& wi,
                             const Intersection& intersection) const {
    vec3 n = intersection.normal;
    if (dot(wo, n) < 0) {
        n = -n;
    }
    
    vec3 h = normalize(wo + wi);
    float hu = dot(h, intersection.tangentU);
    float hv = dot(h, intersection.tangentV);
    float nh = dot(h, n);
    float hk = dot(h, wo);
    
    float specularComponent = sqrt((_roughnessU + 1.f)*(_roughnessV + 1.f)) / (8.f*M_PI);
    float exponent = (_roughnessU*hu*hu +_roughnessV*hv*hv) / (1.f - nh*nh);
    specularComponent *= pow(nh, exponent) / (hk * max(dot(n, wi), dot(n, wo)));
    specularComponent *= _specularIntensity + (1.f - _specularIntensity)*pow(1.f - hk, 5.f);
    
    float diffuseComponent = (28.f*_diffuseIntensity) / (23.f*M_PI);
    diffuseComponent *= (1.f - _specularIntensity);
    diffuseComponent *= (1.f - pow(1.f - (dot(n, wi)/2.f), 5.f));
    diffuseComponent *= (1.f - pow(1.f - (dot(n, wo)/2.f), 5.f));
    
    // Clamp specular component to avoid ridiculously high values
    specularComponent = min(42.f, specularComponent);
    
    return (_diffuseColor*diffuseComponent + _specularColor*specularComponent);
}

Spectrum AshikhminMaterial::sampleBSDF(const vec3& wo, vec3* wi, const Intersection& intersection,
                                       BxDFType) const {
    if ((float)rand()/RAND_MAX < _specularIntensity) {
        float s1 = (float)rand()/RAND_MAX;
        float s2 = (float)rand()/RAND_MAX;
        
        float phi = atanf(sqrt((_roughnessU+1.f)/(_roughnessV+1.f))*tan((M_PI*s1)/2.f));
        
        float quadrant = (float)rand()/RAND_MAX;
        
        if (quadrant >= 0.25f && quadrant < 0.5f) {
            phi = M_PI - phi;
        } else if (quadrant >= 0.5f && quadrant < 0.75f) {
            phi += M_PI;
        } else if (quadrant >= 0.75f) {
            phi = 2.f*M_PI - phi;
        }
        
        float cosphi = cos(phi);
        float sinphi = sin(phi);
        float exponent = 1.f / (_roughnessU * cosphi*cosphi + _roughnessV * sinphi*sinphi + 1.f);
        float theta = acosf(powf(1.f - s2, exponent));
        
        vec3 h = vec3(
                      cos(phi)*sin(theta),
                      cos(theta),
                      sin(phi)*sin(theta)
                      );
        h = normalize(surfaceToWorld(h, intersection));
        *wi = normalize(-wo + 2.f * (dot(wo, h)*h));
        if (dot(intersection.normal, *wi) < 0.f) {
            return Spectrum(0.f);
        }
        return _specularColor;
    } else {
        *wi = normalize(surfaceToWorld(cosineSampleHemisphere(), intersection));
        return _diffuseColor;
    }
}