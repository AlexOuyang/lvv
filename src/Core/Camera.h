//
//  Camera.h
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/3/14.
//
//

#ifndef CSE168_Rendering_Camera_h
#define CSE168_Rendering_Camera_h

#include "CameraSample.h"
#include "Ray.h"
#include "Film.h"

class Camera {
public:
    virtual ~Camera() {};
    
    virtual float generateRay(const CameraSample& sample, Ray* ray) = 0;
    
    Film*   film;
};

#endif
