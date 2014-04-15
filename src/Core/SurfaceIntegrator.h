//
//  SurfaceIntegrator.h
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/4/14.
//
//

#ifndef CSE168_Rendering_SurfaceIntegrator_h
#define CSE168_Rendering_SurfaceIntegrator_h

#include "Core.h"
#include "Integrator.h"
#include "Spectrum.h"

class SurfaceIntegrator : public Integrator {
public:
    virtual ~SurfaceIntegrator() {};
    
    virtual Spectrum li(const Scene& scene, const Renderer& renderer, const Ray& ray,
                        const Intersection& Intersection) const = 0;
};

#endif
