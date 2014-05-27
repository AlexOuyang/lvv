//
//  Sampling.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/23/14.
//
//

#include "Sampling.h"

SamplingConfig SamplingConfig::Load(const rapidjson::Value& value) {
    SamplingConfig config;
    
    if (value.HasMember("count")) {
        config.count = value["count"].GetInt();
    }
    if (value.HasMember("jittered")) {
        config.jittered = value["jittered"].GetBool();
    }
    if (value.HasMember("distribution")) {
        std::string distribution = value["distribution"].GetString();
        if (distribution == "uniform") {
            config.distribution = UniformDistribution;
        } else if (distribution == "gauss") {
            config.distribution = GaussDistribution;
        } else if (distribution == "shirley") {
            config.distribution = ShirleyDistribution;
        }
    }
    
    return config;
}

SamplingConfig::SamplingConfig(int sc, bool jit, SamplingDistribution dist) :
count(sc), jittered(jit), distribution(dist) {
    
}