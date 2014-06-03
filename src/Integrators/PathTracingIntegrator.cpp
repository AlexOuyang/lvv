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

std::shared_ptr<PathTracingIntegrator> PathTracingIntegrator::Load(const rapidjson::Value& value) {
    std::shared_ptr<PathTracingIntegrator> integrator = std::make_shared<PathTracingIntegrator>();
    
    return integrator;
}

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
        if (ray.type == Ray::DiffuseReflected) {
            return l;
        }
        return areaLight->le(ray, &intersection);
    }
    
    l += GetDirectLighting(scene, renderer, ray, intersection);
    
    // Sample BSDF to integrate indirect illumination
    if (ray.depth < _maxRayDepth) {
        
        vec3 wi;
        Material::BxDFType type;
        Spectrum f = intersection.material->sampleBSDF(-ray.direction, &wi, intersection,
                                                       Material::BSDFAll, &type);
        if (!f.isBlack()) {
            Ray reflectedRay(ray);
            
            reflectedRay.origin = intersection.point;
            reflectedRay.direction = wi;
            reflectedRay.tmin = intersection.rayEpsilon;
            reflectedRay.tmax = INFINITY;
            reflectedRay.depth = ray.depth + 1;
            reflectedRay.type = ((type & Material::BSDFDiffuse) ?
                                 Ray::DiffuseReflected : Ray::SpecularReflected);
            Spectrum li = renderer.li(scene, reflectedRay);
            
            l += f * li;
        }
        
    }
    
    return l;
}