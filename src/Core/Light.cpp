//
//  Light.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 5/28/14.
//
//

#include "Core/Light.h"

Light::Light() : _name(), _samplingConfig(1, true, SamplingConfig::UniformDistribution) {
    
}

Light::~Light() {
    
}

void Light::setName(const std::string& name) {
    _name = name;
}

const std::string& Light::getName() const {
    return _name;
}

const SamplingConfig& Light::getSamplingConfig() const {
    return _samplingConfig;
}

void Light::setSamplingConfig(const SamplingConfig& sc) {
    _samplingConfig = sc;
}
