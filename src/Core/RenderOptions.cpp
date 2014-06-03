//
//  RenderOptions.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/4/14.
//
//

#include "RenderOptions.h"

#include <algorithm>

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
        options.surfaceIntegrator = SurfaceIntegrator::Load(value["surfaceIntegrator"]);
        if (!options.surfaceIntegrator) {
            std::cerr << "RenderOptions error: error while loading surface integrator" << std::endl;
        }
    } else {
        std::cerr << "RenderOptions error: no surface integrator given" << std::endl;
    }
    if (value.HasMember("volumeIntegrator")) {
        options.volumeIntegrator = VolumeIntegrator::Load(value["volumeIntegrator"]);
        if (!options.volumeIntegrator) {
            std::cerr << "RenderOptions error: error while loading volume integrator" << std::endl;
        }
    } else {
        std::cerr << "RenderOptions error: no volume integrator given" << std::endl;
    }
    
    return options;
}

RenderOptions::RenderOptions() :
maxThreadsCount(-1),
antialiasingSampling(1, true, SamplingConfig::UniformDistribution),
maxRayDepth(5),
surfaceIntegrator(),
volumeIntegrator() {
    
}

RenderOptions::~RenderOptions() {
    
}