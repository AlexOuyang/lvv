//
//  VolumeIntegrator.h
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 5/15/14.
//
//

#ifndef CSE168_Rendering_VolumeIntegrator_h
#define CSE168_Rendering_VolumeIntegrator_h

#include "Core.h"
#include "Integrator.h"
#include "Spectrum.h"

class VolumeIntegrator : public Integrator {
public:
    virtual ~VolumeIntegrator() {};
    
    virtual Spectrum li(const Scene& scene, const Renderer& renderer,
                        const Ray& ray, Spectrum *transmittance) const = 0;
    virtual Spectrum transmittance(const Scene& scene, const Renderer& renderer,
                                   const Ray& ray) const = 0;
};

#endif
