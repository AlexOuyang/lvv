//
//  SingleScatteringIntegrator.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 5/17/14.
//
//

#include "SingleScatteringIntegrator.h"

#include "Core/Renderer.h"

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
    
    const float stepSize = 0.1f;
    
    float tstart, tend;
    if (volume->intersectP(ray, &tstart, &tend)) {
        float t0 = tstart + ((float)rand()/RAND_MAX)*stepSize;
        float t1 = tstart;
        while (t1 < tend) {
            t1 = t0 + stepSize;
            if (t1 > tend) {
                break;
            }
            vec3 p = ray(t0);
            Ray stepRay = Ray(ray);
            stepRay.tmin = t0;
            stepRay.tmax = t1;
            
            // Compute transmission
            Spectrum tau = volume->tau(stepRay);
            tr *= Spectrum::exp(-tau);
            
            // Add emitted light
            lv += tr * stepSize * volume->le(p);
            
            // Compute radiance contribution from lights
            Spectrum scattering = volume->sigmaS(p) * stepSize;
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
                
                // Multiple scattering
//                if (ray.depth == 0) {
//                    Ray scatteredRay(ray);
//                    
//                    scatteredRay.origin = p;
//                    scatteredRay.tmin = 0.f;
//                    scatteredRay.tmax = INFINITY;
//                    scatteredRay.depth = ray.depth + 1;
//                    
//                    // Uniform sample sphere
//                    float s = (float)rand()/RAND_MAX;
//                    float t = (float)rand()/RAND_MAX;
//                    float u = 2.f*M_PI*s;
//                    float v = sqrt(t*(1.f-t));
//                    scatteredRay.direction.x = 2*v * cos(u);
//                    scatteredRay.direction.y = 1.f - 2.f*t;
//                    scatteredRay.direction.z = 2.f*v * sin(u);
//                    Spectrum scatteredL = renderer.li(scene, scatteredRay);
//                    lv += tr * stepSize * volume->phase(p, -scatteredRay.direction, wo) * scatteredL;
//                }
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