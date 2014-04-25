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

#include <QDebug>

WhittedIntegrator::WhittedIntegrator() {
    
}

WhittedIntegrator::~WhittedIntegrator() {
    
}

Spectrum WhittedIntegrator::li(const Scene& scene, const Renderer& renderer, const Ray& ray,
                               const Intersection& intersection) const {
    Spectrum l(0);
    
    // If primitive is an area light, simply return its emited light
    AreaLight* areaLight = intersection.primitive->getAreaLight();
    if (areaLight) {
        return areaLight->getSpectrum();
    }
    
    // Initialize common variables
    const vec3& point = intersection.point;
    const vec3& n = intersection.normal;
    vec3 wo = -ray.direction;
    
    // Add contribution of each light source
    for (Light* light : scene.lights) {
        // Sample light
        int samplesCount = light->samplingConfig.count;
        for (int i = 0; i < samplesCount; ++i) {
            for (int j = 0; j < samplesCount; ++j) {
                LightSample sample;
                vec3 wi;
                VisibilityTester vt;
                
                sample.u = (float)i/samplesCount;
                sample.v = (float)j/samplesCount;
                
                if (light->samplingConfig.jittered) {
                    sample.u += ((float)rand()/RAND_MAX) / samplesCount;
                    sample.v += ((float)rand()/RAND_MAX) / samplesCount;
                } else {
                    sample.u += 0.5f / samplesCount;
                    sample.v += 0.5f / samplesCount;
                }
                
                Spectrum li = light->sampleL(point, intersection.rayEpsilon, sample, &wi, &vt);
                
                if (li.isBlack()) {
                    continue;
                }
                
                Spectrum f = intersection.material->evaluateBSDF(wo, wi, intersection);
                
                if (!f.isBlack() && vt.unoccluded(scene)) {
                    float cosine = dot(wi, n);
                    if (cosine > 0) {
                        l += li * f * cosine * (1.0f / (samplesCount*samplesCount));
                    }
                }
            }
        }
    }
    
    // Trace rays for specular reflection and refraction
    if (ray.depth < renderer.options.maxRayDepth) {
        
        // Reflectected light
        {
            vec3 wi;
            Spectrum f = intersection.material->sampleBSDF(wo, &wi, intersection,
                                                           Material::BSDFReflection);
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
                                                           Material::BSDFTransmission);
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