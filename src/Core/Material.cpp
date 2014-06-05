//
//  Material.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/23/14.
//
//

#include "Material.h"

#include "Materials/Matte.h"
#include "Materials/Metal.h"
#include "Materials/Glass.h"
#include "Materials/Glossy.h"
#include "Materials/AshikhminMaterial.h"

std::shared_ptr<Material> Material::Load(const rapidjson::Value& value) {
    std::shared_ptr<Material> material;
    
    if (!value.HasMember("type")) {
        return material;
    }
    std::string type = value["type"].GetString();
    std::transform(type.begin(), type.end(), type.begin(), ::tolower);
    if (type == "ashikhmin") {
        material = AshikhminMaterial::Load(value);
    } else if (type == "matte") {
        material = Matte::Load(value);
    } else if (type == "metal") {
        material = Metal::Load(value);
    } else if (type == "glass") {
        material = Glass::Load(value);
    } else if (type == "glossy") {
        material = Glossy::Load(value);
    }
    
    if (value.HasMember("normalMap")) {
        std::shared_ptr<Texture> texture = Texture::Load(value["normalMap"]);
        if (texture) {
            material->setNormalMap(texture);
        }
    }
    
    if (value.HasMember("alphaTexture")) {
        std::shared_ptr<Texture> texture = Texture::Load(value["alphaTexture"]);
        if (texture) {
            material->setAlphaTexture(texture);
        }
    }
    
    return material;
}

Material::Material() : _name(), _normalMap(), _alphaTexture() {
    
}

Material::~Material() {
    
}

const std::string& Material::getName() const {
    return _name;
}

void Material::setName(const std::string& name) {
    _name = name;
}

void Material::setNormalMap(const std::shared_ptr<Texture>& map) {
    _normalMap = map;
}

const Texture* Material::getNormalMap() const {
    return _normalMap.get();
}

void Material::setAlphaTexture(const std::shared_ptr<Texture>& texture) {
    _alphaTexture = texture;
}

const Texture* Material::getAlphaTexture() const {
    return _alphaTexture.get();
}

void Material::setDiffuseColor(const std::shared_ptr<Texture>&) {
}

void Material::setSpecularColor(const std::shared_ptr<Texture>&) {
}

void Material::setDiffuseIntensity(const std::shared_ptr<Texture>&) {
}

void Material::setSpecularIntensity(const std::shared_ptr<Texture>&) {
}

void Material::setDiffuseColor(const vec3&) {
}

void Material::setSpecularColor(const vec3&) {
}

void Material::setDiffuseIntensity(float intensity) {
}

void Material::setSpecularIntensity(float intensity) {
}

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

vec3 Material::cosineSampleHemisphere() {
    // Sample hemisphere
    float s = (float)rand()/RAND_MAX;
    float t = (float)rand()/RAND_MAX;
    float u = 2.0f*M_PI*s;
    float v = sqrt(1.f - t);
    
    return vec3(v*cos(u), sqrt(t), v*sin(u));
}

vec3 Material::surfaceToWorld(const vec3& v, const Intersection& intersection) {
    return normalize(intersection.tangentU*v.x + intersection.tangentV*v.z
                     + intersection.normal*v.y);
}