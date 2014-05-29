//
//  SingleScatteringIntegrator.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 5/17/14.
//
//

#include "SingleScatteringIntegrator.h"

SingleScatteringIntegrator::SingleScatteringIntegrator() {
    
}

SingleScatteringIntegrator::~SingleScatteringIntegrator() {
    
}

Spectrum SingleScatteringIntegrator::li(const Scene& scene, const Renderer& renderer,
                                        const Ray& ray, Spectrum *t) const {
    Volume* volume = scene.getVolume();
    if (!volume) {
        *t = Spectrum(1.f);
        return Spectrum(0.f);
    }
    Spectrum tr = Spectrum(1.f);
    Spectrum lv = Spectrum(0.f);
    
    vec3 wo = -ray.direction;
    
    const float stepSize = 10.f;
    
    float tstart, tend;
    if (volume->intersectP(ray, &tstart, &tend)) {
        float t0 = tstart;
        float t1 = tstart;
        while (t1 < tend) {
            float step = ((float)rand()/RAND_MAX)*stepSize;
            t1 = t0 + step;
            if (t1 > tend) {
                break;
            }
            vec3 p = ray(t0);
            float dist = glm::distance(ray(t0), ray(t1));
            Ray stepRay = Ray(ray);
            stepRay.tmin = t0;
            stepRay.tmax = t1;
            
            // Compute transmission
            Spectrum tau = volume->tau(stepRay);
            tr *= Spectrum::exp(-tau);
            
            // Add emitted light
            lv += tr * dist * volume->le(p);
            
            // Compute radiance contribution from lights
            Spectrum scattering = volume->sigmaS(p) * dist;
            if (!scattering.isBlack()) {
                for (Light* light : scene.getLights()) {
                    VisibilityTester vt(stepRay);
                    LightSample sample;
                    sample.u = (float)rand()/RAND_MAX;
                    sample.v = (float)rand()/RAND_MAX;
                    
                    vec3 wi;
                    Spectrum li = light->sampleL(p, 0.f, sample, &wi, &vt);
                    
                    if (li.isBlack()) {
                        continue;
                    }
                    
                    if (vt.unoccluded(scene)) {
                        li *= scattering * vt.transmittance(scene, renderer);
                        lv += tr * volume->phase(p, -wi, wo) * li;
                    }
                }
            }
            
            t0 = t1;
        }
    }
    
    *t = tr;
    return lv;
}

Spectrum SingleScatteringIntegrator::transmittance(const Scene& scene, const Renderer&,
                                                   const Ray& ray) const {
    Volume* volume = scene.getVolume();
    if (!volume) {
        return Spectrum(1.f);
    }
    Spectrum tau = volume->tau(ray);
    return Spectrum::exp(-tau);
}