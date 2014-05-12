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
_diffuseColor(new UniformVec3Texture(vec3(1.f))),
_specularColor(new UniformVec3Texture(vec3(0.f))),
_diffuseIntensity(new UniformFloatTexture(1.f)),
_specularIntensity(new UniformFloatTexture(0.f)),
_roughnessU(new UniformFloatTexture(1000.f)),
_roughnessV(new UniformFloatTexture(1000.f)) {
    
}

AshikhminMaterial::~AshikhminMaterial() {
    
}

void AshikhminMaterial::setDiffuseColor(Texture *t) {
    _diffuseColor = t;
}

void AshikhminMaterial::setSpecularColor(Texture* t) {
    _specularColor = t;
}

void AshikhminMaterial::setDiffuseIntensity(Texture* t) {
    _diffuseIntensity = t;
}

void AshikhminMaterial::setSpecularIntensity(Texture* t) {
    _specularIntensity = t;
}

void AshikhminMaterial::setRoughness(Texture* tu, Texture* tv) {
    if (tu) {
        _roughnessU = tu;
    }
    if (tv) {
        _roughnessV = tv;
    }
}

void AshikhminMaterial::setDiffuseColor(const vec3& color) {
    UniformVec3Texture* texture = dynamic_cast<UniformVec3Texture*>(_diffuseColor);
    if (texture) {
        texture->setValue(color);
    }
}

void AshikhminMaterial::setSpecularColor(const vec3& color) {
    UniformVec3Texture* texture = dynamic_cast<UniformVec3Texture*>(_specularColor);
    if (texture) {
        texture->setValue(color);
    }
}

void AshikhminMaterial::setDiffuseIntensity(float intensity) {
    UniformFloatTexture* texture = dynamic_cast<UniformFloatTexture*>(_diffuseIntensity);
    if (texture) {
        texture->setValue(intensity);
    }
}

void AshikhminMaterial::setSpecularIntensity(float intensity) {
    UniformFloatTexture* texture = dynamic_cast<UniformFloatTexture*>(_specularIntensity);
    if (texture) {
        texture->setValue(intensity);
    }
}

void AshikhminMaterial::setRoughness(float nu, float nv) {
    UniformFloatTexture* texture = dynamic_cast<UniformFloatTexture*>(_roughnessU);
    if (texture) {
        texture->setValue(nu);
    }
    texture = dynamic_cast<UniformFloatTexture*>(_roughnessV);
    if (texture) {
        texture->setValue(nv);
    }
}

Spectrum AshikhminMaterial::evaluateBSDF(const vec3& wo, const vec3& wi,
                             const Intersection& intersection) const {
    vec3 n = intersection.normal;
    if (dot(wo, n) < 0) {
        n = -n;
    }
    
    float nu = _roughnessU->evaluateFloat(intersection.uv);
    float nv = _roughnessV->evaluateFloat(intersection.uv);
    float diffuseIntensity = _diffuseIntensity->evaluateFloat(intersection.uv);
    float specularIntensity = _specularIntensity->evaluateFloat(intersection.uv);
    vec3 diffuseColor = _diffuseColor->evaluateVec3(intersection.uv);
    vec3 specularColor = _specularColor->evaluateVec3(intersection.uv);
    
    vec3 h = normalize(wo + wi);
    float hu = dot(h, intersection.tangentU);
    float hv = dot(h, intersection.tangentV);
    float nh = dot(h, n);
    float hk = dot(h, wo);
    
    float specularComponent = sqrt((nu + 1.f)*(nv + 1.f)) / (8.f*M_PI);
    float exponent = (nu*hu*hu +nv*hv*hv) / (1.f - nh*nh);
    specularComponent *= pow(nh, exponent) / (hk * max(dot(n, wi), dot(n, wo)));
    specularComponent *= specularIntensity + (1.f - specularIntensity)*pow(1.f - hk, 5.f);
    
    float diffuseComponent = (28.f*diffuseIntensity) / (23.f*M_PI);
    diffuseComponent *= (1.f - specularIntensity);
    diffuseComponent *= (1.f - pow(1.f - (dot(n, wi)/2.f), 5.f));
    diffuseComponent *= (1.f - pow(1.f - (dot(n, wo)/2.f), 5.f));
    
    // Clamp specular component to avoid ridiculously high values
    specularComponent = min(42.f, specularComponent);
    
    return Spectrum(diffuseColor*diffuseComponent + specularColor*specularComponent);
}

Spectrum AshikhminMaterial::sampleBSDF(const vec3& wo, vec3* wi, const Intersection& intersection,
                                       BxDFType) const {
    float nu = _roughnessU->evaluateFloat(intersection.uv);
    float nv = _roughnessV->evaluateFloat(intersection.uv);
    float specularIntensity = _specularIntensity->evaluateFloat(intersection.uv);
    vec3 diffuseColor = _diffuseColor->evaluateVec3(intersection.uv);
    vec3 specularColor = _specularColor->evaluateVec3(intersection.uv);
    
    if ((float)rand()/RAND_MAX < specularIntensity) {
        float s1 = (float)rand()/RAND_MAX;
        float s2 = (float)rand()/RAND_MAX;
        
        float phi = atanf(sqrt((nu+1.f)/(nv+1.f))*tan((M_PI*s1)/2.f));
        
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
        float exponent = 1.f / (nu * cosphi*cosphi + nv * sinphi*sinphi + 1.f);
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
        return specularColor;
    } else {
        *wi = normalize(surfaceToWorld(cosineSampleHemisphere(), intersection));
        return diffuseColor;
    }
}