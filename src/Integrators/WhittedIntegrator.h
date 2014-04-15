//
//  WhittedIntegrator.h
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/4/14.
//
//

#ifndef __CSE168_Rendering__WhittedIntegrator__
#define __CSE168_Rendering__WhittedIntegrator__

#include "Core/SurfaceIntegrator.h"

class WhittedIntegrator : public SurfaceIntegrator {
public:
    
    WhittedIntegrator();
    virtual ~WhittedIntegrator();
    
    virtual Spectrum li(const Scene& scene, const Renderer& renderer, const Ray& ray,
                        const Intersection& Intersection) const;
};

#endif /* defined(__CSE168_Rendering__WhittedIntegrator__) */
