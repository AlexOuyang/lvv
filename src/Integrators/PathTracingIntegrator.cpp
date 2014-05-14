//
//  PathTracingIntegrator.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 5/5/14.
//
//

#include "PathTracingIntegrator.h"

#include "Core/Ray.h"
#include "Core/Scene.h"
#include "Core/Intersection.h"
#include "Core/Material.h"
#include "Core/VisibilityTester.h"
#include "Core/Renderer.h"

PathTracingIntegrator::PathTracingIntegrator() : SurfaceIntegrator() {
    
}

PathTracingIntegrator::~PathTracingIntegrator() {
    
}

Spectrum PathTracingIntegrator::li(const Scene& scene, const Renderer& renderer, const Ray& ray,
                                   const Intersection& intersection) const {
    Spectrum l(0.f);
    
    // If primitive is an area light, simply return its emited light
    AreaLight* areaLight = intersection.primitive->getAreaLight();
    if (areaLight) {
        return areaLight->le(ray);
    }
    
    // Initialize common variables
    const vec3& point = intersection.point;
    vec3 n = intersection.normal;
    vec3 wo = -ray.direction;
    
    if (dot(n, wo) < 0) {
        n = -n;
    }
    
    // Add contribution of each light source
    for (Light* light : scene.getLights()) {
        // Sample light
        const SamplingConfig& sampling = light->getSamplingConfig();
        int samplesCount = sampling.count;
        for (int i = 0; i < samplesCount; ++i) {
            for (int j = 0; j < samplesCount; ++j) {
                LightSample sample;
                vec3 wi;
                VisibilityTester vt(ray);
                
                sample.u = (float)i/samplesCount;
                sample.v = (float)j/samplesCount;
                
                if (sampling.jittered) {
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
                
                float cosine = dot(wi, n);
                if (cosine < 0) {
                    continue;
                }
                
                Spectrum f = intersection.material->evaluateBSDF(wo, wi, intersection);
                
                if (cosine > 0 && !f.isBlack() && vt.unoccluded(scene)) {
                    l += li * f * cosine * (1.0f / (samplesCount*samplesCount));
                }
            }
        }
    }
    
    // Sample BSDF to integrate indirect illumination
    if (ray.depth < renderer.options.maxRayDepth) {
        
        vec3 wi;
        Spectrum f = intersection.material->sampleBSDF(wo, &wi, intersection,
                                                       Material::BSDFAll);
        if (!f.isBlack() && glm::dot(wi, n) != 0.0f) {
            Ray reflectedRay(ray);
            
            reflectedRay.origin = intersection.point;
            reflectedRay.direction = wi;
            reflectedRay.tmin = intersection.rayEpsilon;
            reflectedRay.tmax = INFINITY;
            reflectedRay.depth = ray.depth + 1;
            Spectrum li = renderer.li(scene, reflectedRay);
            l += f * li;
        }
        
    }
    
    return l;
}