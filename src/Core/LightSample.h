//
//  LightSample.h
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/23/14.
//
//

#ifndef __CSE168_Rendering__LightSample__
#define __CSE168_Rendering__LightSample__

#include "Core.h"

class LightSample {
public:
    
    LightSample(float u=0.0f, float v =0.0f);
    ~LightSample();
    
    float u, v;
};

#endif /* defined(__CSE168_Rendering__LightSample__) */
