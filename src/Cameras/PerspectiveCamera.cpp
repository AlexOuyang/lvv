//
//  PerspectiveCamera.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/3/14.
//
//

#include "PerspectiveCamera.h"

#include "Core/ConfigFileReader.h"

std::shared_ptr<PerspectiveCamera> PerspectiveCamera::Load(const rapidjson::Value& value) {
    // Check if mandatory values are specified
    if (!value.HasMember("position")) {
        std::cerr << "Camera error: no position specified" << std::endl;
        return std::shared_ptr<PerspectiveCamera>();
    }
    if (!value.HasMember("target")) {
        std::cerr << "Camera error: no target specified" << std::endl;
        return std::shared_ptr<PerspectiveCamera>();
    }
    if (!value.HasMember("vfov")) {
        std::cerr << "Camera error: no vertical fov specified" << std::endl;
        return std::shared_ptr<PerspectiveCamera>();
    }
    if (!value.HasMember("aspect")) {
        std::cerr << "Camera error: no aspect ratio specified" << std::endl;
        return std::shared_ptr<PerspectiveCamera>();
    }
    
    std::shared_ptr<PerspectiveCamera> camera = std::make_shared<PerspectiveCamera>();
    
    vec3 position = ConfigFileReader::LoadVec3(value["position"]);
    vec3 target = ConfigFileReader::LoadVec3(value["target"]);
    vec3 upVector = vec3(0.f, 1.f, 0.f);
    if (value.HasMember("upVector")) {
        upVector = ConfigFileReader::LoadVec3(value["upVector"]);
    }
    
    camera->lookAt(position, target, upVector);
    camera->setVFov(value["vfov"].GetDouble());
    camera->setAspect(value["aspect"].GetDouble());
    
    return camera;
}

PerspectiveCamera::PerspectiveCamera() :
_vfov(glm::radians(40.f)), _aspect(1.33f), _focusDistance(1.f), _apertureSize(0.f) {

}

PerspectiveCamera::~PerspectiveCamera() {
    
}

float PerspectiveCamera::generateRay(const CameraSample& sample, Ray* ray) {
    float time = (float)rand()/RAND_MAX;
    
    Transform t = _transform.interpolate(time);
    vec3
        a(t.getMatrixColumn(0)), b(t.getMatrixColumn(1)),
        c(t.getMatrixColumn(2)), d(t.getMatrixColumn(3))
    ;
    
    float planeHeight = 2*tanf(_vfov/2);
    float planeWidth = planeHeight * _aspect;
    
    ray->origin = d;
    
    // Plane center
    ray->direction = ray->origin - c;

    // Point on plane
    ray->direction += (a*planeWidth*sample.position.x - a*(planeWidth/2)
                       + b*planeHeight*sample.position.y - b*(planeHeight/2));
    
    ray->direction = ray->direction - ray->origin;
    
    // Add depth of field
    if (_apertureSize > 0.f) {
        vec3 focusPoint = ray->origin + ray->direction*_focusDistance;
        
        // Sample in a disc
        float s = (float)rand()/RAND_MAX;
        float t = (float)rand()/RAND_MAX;
        float x = sqrt(t) * cos(2.f*M_PI*s);
        float y = sqrt(t) * sin(2.f*M_PI*s);
        
        ray->origin += _apertureSize*(x*a + y*b);
        ray->direction = normalize(focusPoint - ray->origin);
    } else {
        ray->direction = normalize(ray->direction);
    }
    
    ray->time = time;
    
    return 1.0;
}

void PerspectiveCamera::lookAt(const vec3& position, const vec3& target, const vec3& up) {
    Transform t;
    t.lookAt(position, target, up);
    _transform.setTransform(t);
}

void PerspectiveCamera::setVFov(float vfov) {
    // Convert to radians
    _vfov = radians(vfov);
}

void PerspectiveCamera::setAspect(float aspect) {
    _aspect = aspect;
}

void PerspectiveCamera::setFocusDistance(float d) {
    _focusDistance = d;
}

void PerspectiveCamera::setFocusPoint(const vec3& point) {
    vec3 pos = vec3(_transform[0].getMatrixColumn(3));
    _focusDistance = glm::distance(pos, point);
}

void PerspectiveCamera::setApertureSize(float aperture) {
    _apertureSize = aperture;
}