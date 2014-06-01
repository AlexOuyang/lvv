//
//  PathTracingIntegrator.h
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 5/5/14.
//
//

#ifndef __CSE168_Rendering__PathTracingIntegrator__
#define __CSE168_Rendering__PathTracingIntegrator__

#include "Core/Core.h"
#include "Core/SurfaceIntegrator.h"

class PathTracingIntegrator : public SurfaceIntegrator {
public:
    
    PathTracingIntegrator();
    virtual ~PathTracingIntegrator();
    
    virtual Spectrum li(const Scene& scene, const Renderer& renderer, const Ray& ray,
                        const Intersection& Intersection) const;
};

#endif /* defined(__CSE168_Rendering__PathTracingIntegrator__) */
