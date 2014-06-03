//
//  Integrator.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 6/1/14.
//
//

#include "Core/Integrator.h"

#include "Core/Scene.h"
#include "Core/Light.h"
#include "Core/Material.h"

Integrator::~Integrator() {
    
}

void Integrator::preprocess(const Scene&, const Camera*, const Renderer&) {
    
}

Spectrum Integrator::GetDirectLighting(const Scene& scene, const Renderer& renderer,
                                       const Ray& ray, const Intersection& intersection) {
    Spectrum l(0.f);
    
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
                
                // Apply attenuation from scene volumes
                li *= vt.transmittance(scene, renderer);
                
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
    return l;
}