//
//  RenderOptions.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/4/14.
//
//

#include "RenderOptions.h"

#include "Integrators/WhittedIntegrator.h"
#include "Integrators/PathTracingIntegrator.h"
#include "Integrators/SingleScatteringIntegrator.h"

RenderOptions::RenderOptions() :
maxThreadsCount(-1),
antialiasingSampling(1, true, SamplingConfig::ShirleyDistribution),
maxRayDepth(5) {
    
}

RenderOptions::~RenderOptions() {
    
}

SurfaceIntegrator* RenderOptions::createSurfaceIntegrator() {
    return new PathTracingIntegrator();
    //return new WhittedIntegrator();
}

VolumeIntegrator* RenderOptions::createVolumeIntegrator() {
    return new SingleScatteringIntegrator();
}