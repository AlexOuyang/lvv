//
//  AnimationEvaluator.h
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 6/4/14.
//
//

#ifndef __CSE168_Rendering__AnimationEvaluator__
#define __CSE168_Rendering__AnimationEvaluator__

#include "Core.h"

class AnimationEvaluator {
public:
    
    AnimationEvaluator();
    virtual ~AnimationEvaluator();
    
    virtual void evaluate(float tstart, float tend) = 0;
};

#endif /* defined(__CSE168_Rendering__AnimationEvaluator__) */
