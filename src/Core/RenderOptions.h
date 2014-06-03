//
//  RenderingOptions.h
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/3/14.
//
//

#ifndef CSE168_Rendering_RenderOptions_h
#define CSE168_Rendering_RenderOptions_h

#include "Core.h"
#include "SurfaceIntegrator.h"
#include "VolumeIntegrator.h"
#include "Sampling.h"

class RenderOptions {
public:
    
    static RenderOptions Load(const rapidjson::Value& value);
    
    RenderOptions();
    ~RenderOptions();
    
    int                                 maxThreadsCount;
    SamplingConfig                      antialiasingSampling;
    int                                 maxRayDepth;
    std::shared_ptr<SurfaceIntegrator>  surfaceIntegrator;
    std::shared_ptr<VolumeIntegrator>   volumeIntegrator;
};

#endif
