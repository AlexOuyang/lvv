//
//  Transform.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/13/14.
//
//

#include "Transform.h"

#include "Ray.h"

Transform Transform::Inverse(const Transform& t) {
    Transform result;
    
    result.m = glm::inverse(t.m);
    return result;
}

Transform::Transform() : m() {
    
}

Transform::Transform(const mat4x4& m) : m(m) {
    
}

void Transform::lookAt(const vec3& position, const vec3& target, const vec3& up) {
    vec3 d = position;
    vec3 c = normalize(d - target);
    vec3 a = normalize(cross(up, c));
    vec3 b = cross(c, a);
    m[0] = vec4(a, 0.0);
    m[1] = vec4(b, 0.0);
    m[2] = vec4(c, 0.0);
    m[3] = vec4(d, 1.0);
}

vec3 Transform::applyToVector(const vec3& v) const {
    return vec3(m * vec4(v, 0.0f));
}

vec3 Transform::applyToNormal(const vec3& n) const {
    mat4x4 normalMatrix = inverse(transpose(m));
    return vec3(normalMatrix * vec4(n, 0.0f));
}

vec3 Transform::operator()(const vec3& p) const {
    return vec3(m * vec4(p, 1.0f));
}

Ray Transform::operator()(const Ray& r) const {
    Ray result(r);
    
    result.origin = (*this)(r.origin);
    result.direction = applyToVector(r.direction);
    return result;
}

AABB Transform::operator()(const AABB& box) const {
    const Transform& t = *this;
    
    AABB result = AABB(t(vec3(box.min.x, box.min.y, box.min.z)));
    result = AABB::Union(result, t(vec3(box.max.x, box.min.y, box.min.z)));
    result = AABB::Union(result, t(vec3(box.min.x, box.max.y, box.min.z)));
    result = AABB::Union(result, t(vec3(box.min.x, box.min.y, box.max.z)));
    result = AABB::Union(result, t(vec3(box.min.x, box.max.y, box.max.z)));
    result = AABB::Union(result, t(vec3(box.max.x, box.min.y, box.max.z)));
    result = AABB::Union(result, t(vec3(box.max.x, box.max.y, box.min.z)));
    result = AABB::Union(result, t(vec3(box.max.x, box.max.y, box.max.z)));
    return result;
}