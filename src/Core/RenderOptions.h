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
    
    enum SurfaceIntegrator {
        WhittedIntegrator,
        PathTracingIntegrator,
        PhotonMappingIntegrator
    };
    
    enum VolumeIntegrator {
        SingleScatteringIntegrator
    };
    
    static RenderOptions Load(const rapidjson::Value& value);
    
    RenderOptions();
    ~RenderOptions();
    
    ::SurfaceIntegrator* createSurfaceIntegrator();
    ::VolumeIntegrator* createVolumeIntegrator();
    
    int                 maxThreadsCount;
    SamplingConfig      antialiasingSampling;
    int                 maxRayDepth;
    SurfaceIntegrator   surfaceIntegrator;
    VolumeIntegrator    volumeIntegrator;
};

#endif
