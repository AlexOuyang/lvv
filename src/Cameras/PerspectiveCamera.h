//
//  PerspectiveCamera.h
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/3/14.
//
//

#ifndef __CSE168_Rendering__PerspectiveCamera__
#define __CSE168_Rendering__PerspectiveCamera__

#include "Core/Camera.h"

class PerspectiveCamera : public Camera {
public:
    PerspectiveCamera();
    virtual ~PerspectiveCamera();
    
    virtual float generateRay(const CameraSample& sample, Ray* ray);
    
    void setMatrix(const mat4x4& matrix);
    void lookAt(const vec3& position, const vec3& target, const vec3& up=vec3(0, 1, 0));
    void setVFov(float vfov);
    void setAspect(float aspect);
  
private:
    mat4x4  _matrix;
    float   _vfov;
    float   _aspect;
};

#endif /* defined(__CSE168_Rendering__PerspectiveCamera__) */
