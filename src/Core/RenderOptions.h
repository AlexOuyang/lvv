//
//  RenderingOptions.h
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/3/14.
//
//

#ifndef CSE168_Rendering_RenderOptions_h
#define CSE168_Rendering_RenderOptions_h

#include "SurfaceIntegrator.h"

class RenderOptions {
public:
    RenderOptions();
    ~RenderOptions();
    
    SurfaceIntegrator* createSurfaceIntegrator();
};

#endif
