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
#include "Core/AnimatedTransform.h"

class PerspectiveCamera : public Camera {
public:
    
    static std::shared_ptr<PerspectiveCamera> Load(const rapidjson::Value& value);
    
    PerspectiveCamera();
    virtual ~PerspectiveCamera();
    
    virtual float generateRay(const CameraSample& sample, Ray* ray);

    void setTransform(const AnimatedTransform& t);
    void setTransform(const Transform& t);
    void lookAt(const vec3& position, const vec3& target, const vec3& up=vec3(0, 1, 0));
    
    void setVFov(float vfov);
    void setAspect(float aspect);
    void setFocusDistance(float d);
    void setFocusPoint(const vec3& point);
    void setApertureSize(float aperture);
  
private:
    AnimatedTransform   _transform;
    float               _vfov;
    float               _aspect;
    float               _focusDistance;
    float               _apertureSize;
};

#endif /* defined(__CSE168_Rendering__PerspectiveCamera__) */
