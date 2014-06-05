//
//  AnimatedTriangle.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 6/5/14.
//
//

#include "AnimatedTriangle.h"

#include "Core/Ray.h"
#include "Shapes/AnimatedMesh.h"
#include "Core/Material.h"

AnimatedTriangle::AnimatedTriangle() : _vertices(), _mesh(), _material() {
    
}

AnimatedTriangle::AnimatedTriangle(uint_t a, uint_t b, uint_t c) : _vertices(), _mesh(), _material() {
    _vertices[0] = a;
    _vertices[1] = b;
    _vertices[2] = c;
}

AnimatedTriangle::~AnimatedTriangle() {
}

void AnimatedTriangle::setMesh(const std::shared_ptr<const AnimatedMesh>& mesh) {
    _mesh = mesh;
}

void AnimatedTriangle::setMaterial(const std::shared_ptr<Material>& material) {
    _material = material;
}

bool AnimatedTriangle::intersect(const Ray& ray, Intersection* intersection) const {
    Vertex v0 = _mesh->getInterpolatedVertex(_vertices[0], ray.time);
    Vertex v1 = _mesh->getInterpolatedVertex(_vertices[1], ray.time);
    Vertex v2 = _mesh->getInterpolatedVertex(_vertices[2], ray.time);
    
    const vec3 &a = v0.position, &b = v1.position, &c = v2.position;
    vec3 oa = ray.origin - a, ca = c - a, ba = b - a;
    vec3 normal = cross(ba, ca);
    float det = dot(-ray.direction, normal);
    
    // Determinant null
    if (det == 0.0f) {
        return false;
    }
    
    float alpha = dot(-ray.direction, cross(oa, ca)) / det;
    if (alpha <= 0 || alpha >= 1) {
        return false;
    }
    
    float beta = dot(-ray.direction, cross(ba, oa)) / det;
    if (beta <= 0 || beta >= 1) {
        return false;
    }
    
    if (alpha + beta >= 1) {
        return false;
    }
    
    float t = dot(oa, normal) / det;
    if (t < ray.tmin || t > ray.tmax) {
        return false;
    }
    
    // Compute uv coords
    vec2 uvs = ((1-alpha-beta)*v0.texCoord
                + alpha*v1.texCoord
                + beta*v2.texCoord);
    
    // Reject if we have an alpha texture
    const Texture* alphaTexture = nullptr;
    if (_mesh && _mesh->_alphaTexture) {
        alphaTexture = _mesh->_alphaTexture.get();
    } else if (_material) {
        alphaTexture = _material->getAlphaTexture();
    }
    if (alphaTexture && alphaTexture->evaluateFloat(uvs) < 0.1f) {
        return false;
    }
    
    // Compute smoothed normal
    normal = ((1-alpha-beta)*v0.normal
              + alpha*v1.normal
              + beta*v2.normal);
    
    // Update the ray
    ray.tmax = t;
    
    // Fill in intersection informations
    intersection->t = t;
    intersection->point = ray(t);
    intersection->normal = normal;
    intersection->uv = uvs;
    
    // Interpolate tangents
    intersection->tangentU = ((1-alpha-beta)*v0.tangentU
                              + alpha*v1.tangentU
                              + beta*v2.tangentU);
    intersection->tangentV = ((1-alpha-beta)*v0.tangentV
                              + alpha*v1.tangentV
                              + beta*v2.tangentV);
    
    if (_material) {
        intersection->material = _material.get();
    }
    
    intersection->rayEpsilon = 1e-3f * t;
    return true;
}

bool AnimatedTriangle::intersectP(const Ray& ray) const {
    Vertex v0 = _mesh->getInterpolatedVertex(_vertices[0], ray.time);
    Vertex v1 = _mesh->getInterpolatedVertex(_vertices[1], ray.time);
    Vertex v2 = _mesh->getInterpolatedVertex(_vertices[2], ray.time);
    
    const vec3 &a = v0.position, &b = v1.position, &c = v2.position;
    vec3 oa = ray.origin - a, ca = c - a, ba = b - a;
    vec3 normal = cross(ba, ca);
    float det = dot(-ray.direction, normal);
    
    // Determinant null
    if (det == 0.0f) {
        return false;
    }
    
    float alpha = dot(-ray.direction, cross(oa, ca)) / det;
    if (alpha <= 0 || alpha >= 1) {
        return false;
    }
    
    float beta = dot(-ray.direction, cross(ba, oa)) / det;
    if (beta <= 0 || alpha + beta >= 1) {
        return false;
    }
    
    float t = dot(oa, normal) / det;
    if (t < ray.tmin || t > ray.tmax) {
        return false;
    }
    
    // Compute uv coords
    vec2 uvs = ((1-alpha-beta)*v0.texCoord
                + alpha*v1.texCoord
                + beta*v2.texCoord);
    
    // Reject if we have an alpha texture
    if (_mesh && _mesh->_alphaTexture) {
        if (_mesh->_alphaTexture->evaluateFloat(uvs) == 0.0f) {
            return false;
        }
    }
    
    return true;
}

AABB AnimatedTriangle::getBoundingBox() const {
    AABB b1 = AABB::Union(AABB(_mesh->getVertex(_vertices[0])->position,
                               _mesh->getVertex(_vertices[1])->position),
                          _mesh->getVertex(_vertices[2])->position);
    AABB b2 = AABB::Union(AABB(_mesh->getEndVertex(_vertices[0])->position,
                               _mesh->getEndVertex(_vertices[1])->position),
                          _mesh->getEndVertex(_vertices[2])->position);
    return AABB::Union(b1, b2);
}

bool AnimatedTriangle::hasMaterial() const {
    return (bool)_material;
}