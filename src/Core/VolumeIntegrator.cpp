//
//  VolumeIntegrator.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 6/2/14.
//
//

#include "Core/VolumeIntegrator.h"

#include "Integrators/SingleScatteringIntegrator.h"

std::shared_ptr<VolumeIntegrator> VolumeIntegrator::Load(const rapidjson::Value& value) {
    std::shared_ptr<VolumeIntegrator> integrator;
    
    std::string type;
    
    if (value.IsString()) {
        type = value.GetString();
    } else if (value.IsObject() && value.HasMember("type")) {
        type = value["type"].GetString();
    } else {
        std::cerr << "VolumeIntegrator error: no type given" << std::endl;
        return integrator;
    }
    
    std::transform(type.begin(), type.end(), type.begin(), ::tolower);
    
    if (type == "singlescattering") {
        integrator = SingleScatteringIntegrator::Load(value);
    } else {
        std::cerr << "VolumeIntegrator error: unknown type \"" << type << "\"" << std::endl;
        return integrator;
    }
    
    return integrator;
}

VolumeIntegrator::~VolumeIntegrator() {
    
}