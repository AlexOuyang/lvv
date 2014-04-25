//
//  PerspectiveCamera.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/3/14.
//
//

#include "PerspectiveCamera.h"

PerspectiveCamera::PerspectiveCamera() {

}

PerspectiveCamera::~PerspectiveCamera() {
    
}

float PerspectiveCamera::generateRay(const CameraSample& sample, Ray* ray) {
    vec3 a(_matrix[0]), b(_matrix[1]), c(_matrix[2]), d(_matrix[3]);
    float planeHeight = 2*tanf(_vfov/2);
    float planeWidth = planeHeight * _aspect;
    
    ray->origin = d;
    
    // Plane center
    ray->direction = ray->origin - c;

    // Point on plane
    ray->direction += (a*planeWidth*sample.position.x - a*(planeWidth/2)
                       + b*planeHeight*sample.position.y - b*(planeHeight/2));
    
    ray->direction = normalize(ray->direction - ray->origin);
    
    return 1.0;
}

void PerspectiveCamera::setMatrix(const mat4x4& matrix) {
    _matrix = matrix;
}

void PerspectiveCamera::lookAt(const vec3& position, const vec3& target, const vec3& up) {
    vec3 d = position;
    vec3 c = normalize(d - target);
    vec3 a = normalize(cross(up, c));
    vec3 b = cross(c, a);
    _matrix[0] = vec4(a, 0.0);
    _matrix[1] = vec4(b, 0.0);
    _matrix[2] = vec4(c, 0.0);
    _matrix[3] = vec4(d, 1.0);
}

void PerspectiveCamera::setVFov(float vfov) {
    // Convert to radians
    _vfov = radians(vfov);
}

void PerspectiveCamera::setAspect(float aspect) {
    _aspect = aspect;
}