//
//  SurfaceIntegrator.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 6/2/14.
//
//

#include "Core/SurfaceIntegrator.h"

#include "Integrators/PathTracingIntegrator.h"
#include "Integrators/PhotonMappingIntegrator.h"
#include "Integrators/WhittedIntegrator.h"

std::shared_ptr<SurfaceIntegrator> SurfaceIntegrator::Load(const rapidjson::Value& value) {
    std::shared_ptr<SurfaceIntegrator> integrator;
    
    std::string type;
    
    if (value.IsString()) {
        type = value.GetString();
    } else if (value.IsObject() && value.HasMember("type")) {
        type = value["type"].GetString();
    } else {
        std::cerr << "SurfaceIntegrator error: no type given" << std::endl;
        return integrator;
    }
    
    std::transform(type.begin(), type.end(), type.begin(), ::tolower);
    
    if (type == "pathtracing") {
        integrator = PathTracingIntegrator::Load(value);
    } else if (type == "photonmapping") {
        integrator = PhotonMappingIntegrator::Load(value);
    } else if (type == "whitted") {
        integrator = WhittedIntegrator::Load(value);
    } else {
        std::cerr << "SurfaceIntegrator error: unknown type \"" << type << "\"" << std::endl;
        return integrator;
    }
    
    if (value.HasMember("maxRayDepth")) {
        integrator->setMaxRayDepth(value["maxRayDepth"].GetInt());
    }
    
    return integrator;
}

SurfaceIntegrator::SurfaceIntegrator() : _maxRayDepth(5) {
    
}

void SurfaceIntegrator::setMaxRayDepth(uint_t depth) {
    _maxRayDepth = depth;
}

SurfaceIntegrator::~SurfaceIntegrator() {
    
}