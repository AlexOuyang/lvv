//
//  Sampling.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/23/14.
//
//

#include "Sampling.h"

SamplingConfig::SamplingConfig(int sc, bool jit, SamplingDistribution dist) :
count(sc), jittered(jit), distribution(dist) {
    
}