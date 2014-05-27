//
//  Sampling.h
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/23/14.
//
//

#ifndef __CSE168_Rendering__Sampling__
#define __CSE168_Rendering__Sampling__

#include "Core.h"

struct SamplingConfig {
    
    static SamplingConfig Load(const rapidjson::Value& value);
    
    enum SamplingDistribution {
        UniformDistribution,
        GaussDistribution,
        ShirleyDistribution
    };
    
    SamplingConfig(int sc=1, bool jit=true,
                   SamplingDistribution dist=UniformDistribution);
    
    int                     count;
    bool                    jittered;
    SamplingDistribution    distribution;
};

#endif /* defined(__CSE168_Rendering__Sampling__) */
