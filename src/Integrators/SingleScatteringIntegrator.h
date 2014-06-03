//
//  SingleScatteringIntegrator.h
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 5/17/14.
//
//

#ifndef __CSE168_Rendering__SingleScatteringIntegrator__
#define __CSE168_Rendering__SingleScatteringIntegrator__

#include "Core/Core.h"
#include "Core/VolumeIntegrator.h"
#include "Core/Scene.h"

class SingleScatteringIntegrator : public VolumeIntegrator {
public:
    
    static std::shared_ptr<SingleScatteringIntegrator> Load(const rapidjson::Value& value);
    
    SingleScatteringIntegrator();
    virtual ~SingleScatteringIntegrator();
    
    virtual Spectrum li(const Scene& scene, const Renderer& renderer,
                        const Ray& ray, Spectrum *t) const;
    virtual Spectrum transmittance(const Scene& scene, const Renderer& renderer,
                                   const Ray& ray) const;
    
private:
    
};

#endif /* defined(__CSE168_Rendering__SingleScatteringIntegrator__) */
