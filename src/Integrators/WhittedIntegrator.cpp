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

WhittedIntegrator::WhittedIntegrator() {
    
}

WhittedIntegrator::~WhittedIntegrator() {
    
}

Spectrum WhittedIntegrator::li(const Scene& scene, const Renderer&, const Ray& ray,
                               const Intersection& intersection) const {
    Spectrum l(0);
    
    // Initialize common variables
    const vec3& point = intersection.point;
    const vec3& n = intersection.normal;
    vec3 wo = -ray.direction;
    
    // Add contribution of each light source
    for (Light* light : scene.lights) {
        vec3 wi;
        VisibilityTester vt;
        
        Spectrum li = light->sampleL(point, intersection.rayEpsilon, &wi, &vt);
        
        if (li.isBlack()) {
            continue;
        }
        
        Spectrum bsdf = intersection.material->evaluateBSDF(wo, wi);
        
        if (!bsdf.isBlack() && vt.unoccluded(scene)) {
            float cosine = dot(wi, n);
            if (cosine > 0) {
                l += li * bsdf * cosine;
            }
        }
        
    }
    
//    // Reflection test
//    if (ray.depth < 5) {
//        Ray reflected;
//        reflected.depth = ray.depth + 1;
//        reflected.origin = vec3;
//        reflected.direction = reflect(ray.direction, n);
//        Spectrum lr = renderer.li(scene, reflected);
//        
//        l += 0.5 * lr;
//    }
    
    return l;
}