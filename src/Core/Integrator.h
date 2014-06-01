//
//  Integrator.h
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/4/14.
//
//

#ifndef CSE168_Rendering_Integrator_h
#define CSE168_Rendering_Integrator_h

#include "Core.h"

class Integrator {
public:    
    virtual ~Integrator() {};
    
    virtual void preprocess(const Scene&, const Camera*) {};
};

#endif
