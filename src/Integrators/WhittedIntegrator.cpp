//
//  WhittedIntegrator.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/4/14.
//
//

#include "WhittedIntegrator.h"

#include "Core/Ray.h"
#include "Core/Scene.h"
#include "Core/Intersection.h"
#include "Core/Material.h"
#include "Core/VisibilityTester.h"
#include "Core/Renderer.h"

std::shared_ptr<WhittedIntegrator> WhittedIntegrator::Load(const rapidjson::Value& value) {
    std::shared_ptr<WhittedIntegrator> integrator = std::make_shared<WhittedIntegrator>();
    
    return integrator;
}

WhittedIntegrator::WhittedIntegrator() {
    
}

WhittedIntegrator::~WhittedIntegrator() {
    
}

Spectrum WhittedIntegrator::li(const Scene& scene, const Renderer& renderer, const Ray& ray,
                               const Intersection& intersection) const {
    Spectrum l(0.f);
    
    // If primitive is an area light, simply return its emited light
    AreaLight* areaLight = intersection.primitive->getAreaLight();
    if (areaLight) {
        return areaLight->le(ray, &intersection);
    }
    
    // Initialize common variables
    const vec3& n = intersection.normal;
    vec3 wo = -ray.direction;
    
    l += GetDirectLighting(scene, renderer, ray, intersection);
    
    // Trace rays for specular reflection and refraction
    if (ray.depth < _maxRayDepth) {

        Material::BxDFType type;
        // Reflected light
        {
            vec3 wi;
            Spectrum f = intersection.material->sampleBSDF(wo, &wi, intersection,
                                                           Material::BSDFReflection, &type);
            if (!f.isBlack() && glm::dot(wi, n) != 0.0f) {
                Ray reflectedRay;
                
                reflectedRay.origin = intersection.point;
                reflectedRay.direction = wi;
                reflectedRay.tmin = intersection.rayEpsilon;
                reflectedRay.depth = ray.depth + 1;
                Spectrum li = renderer.li(scene, reflectedRay);
                l += f * li;
            }
        }
        
        // Transmitted light
        {
            vec3 wi;
            Spectrum f = intersection.material->sampleBSDF(wo, &wi, intersection,
                                                           Material::BSDFTransmission, &type);
            if (!f.isBlack() && glm::dot(wi, n) != 0.0f) {
                Ray transmittedRay;
                
                transmittedRay.origin = intersection.point;
                transmittedRay.direction = wi;
                transmittedRay.tmin = intersection.rayEpsilon;
                transmittedRay.depth = ray.depth + 1;
                Spectrum li = renderer.li(scene, transmittedRay);
                
                // Absorbtion
                if (glm::dot(wi, n) < 0 && transmittedRay.tmax != INFINITY) {
                    li = li * intersection.material->transmittedLight(transmittedRay.tmax);
                }
                l += f * li;
            }
        }
        
    }
    
    return l;
}