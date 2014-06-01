//
//  RenderOptions.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/4/14.
//
//

#include "RenderOptions.h"

#include <algorithm>

#include "Integrators/WhittedIntegrator.h"
#include "Integrators/PathTracingIntegrator.h"
#include "Integrators/PhotonMappingIntegrator.h"
#include "Integrators/SingleScatteringIntegrator.h"

RenderOptions RenderOptions::Load(const rapidjson::Value& value) {
    RenderOptions options;
    
    if (value.HasMember("maxThreadsCount")) {
        options.maxThreadsCount = value["maxThreadsCount"].GetInt();
    }
    if (value.HasMember("antialiasingSampling")) {
        options.antialiasingSampling = SamplingConfig::Load(value["antialiasingSampling"]);
    }
    if (value.HasMember("maxRayDepth")) {
        options.maxRayDepth = value["maxRayDepth"].GetInt();
    }
    if (value.HasMember("surfaceIntegrator")) {
        std::string str = value["surfaceIntegrator"].GetString();
        std::transform(str.begin(), str.end(), str.begin(), ::tolower);
        
        if (str == "whitted") {
            options.surfaceIntegrator = WhittedIntegrator;
        } else if (str == "pathtracing") {
            options.surfaceIntegrator = PathTracingIntegrator;
        } else if (str == "photonmapping") {
            options.surfaceIntegrator = PhotonMappingIntegrator;
        }
    }
    if (value.HasMember("volumeIntegrator")) {
        std::string str = value["volumeIntegrator"].GetString();
        std::transform(str.begin(), str.end(), str.begin(), ::tolower);
        
        if (str == "singlescattering") {
            options.volumeIntegrator = SingleScatteringIntegrator;
        }
    }
    return options;
}

RenderOptions::RenderOptions() :
maxThreadsCount(-1),
antialiasingSampling(1, true, SamplingConfig::ShirleyDistribution),
maxRayDepth(5),
surfaceIntegrator(PathTracingIntegrator),
volumeIntegrator(SingleScatteringIntegrator) {
    
}

RenderOptions::~RenderOptions() {
    
}

::SurfaceIntegrator* RenderOptions::createSurfaceIntegrator() {
    if (surfaceIntegrator == WhittedIntegrator) {
        return new ::WhittedIntegrator();
    } else if (surfaceIntegrator == PathTracingIntegrator) {
        return new ::PathTracingIntegrator();
    } else if (surfaceIntegrator == PhotonMappingIntegrator) {
        return new ::PhotonMappingIntegrator();
    }
    return nullptr;
}

::VolumeIntegrator* RenderOptions::createVolumeIntegrator() {
    if (volumeIntegrator == SingleScatteringIntegrator) {
        return new ::SingleScatteringIntegrator();
    }
    return nullptr;
}