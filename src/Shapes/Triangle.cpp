//
//  Triangle.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/4/14.
//
//

#include "Triangle.h"

#include "Core/Ray.h"
#include "Shapes/Mesh.h"

Triangle::Triangle() : _vertices(), _mesh() {
    
}

Triangle::Triangle(Vertex* a, Vertex* b, Vertex* c) : _vertices(), _mesh() {
    _vertices[0] = a;
    _vertices[1] = b;
    _vertices[2] = c;    
}

Triangle::~Triangle() {
}

void Triangle::setMesh(const std::shared_ptr<const Mesh>& mesh) {
    _mesh = mesh;
}

void Triangle::setVertices(Vertex* v1, Vertex* v2, Vertex* v3) {
    _vertices[0] = v1;
    _vertices[1] = v2;
    _vertices[2] = v3;
}

Vertex* Triangle::getVertex(int num) const {
    return _vertices[num];
}

bool Triangle::intersect(const Ray& ray, Intersection* intersection) const {
    const vec3 &a = _vertices[0]->position, &b = _vertices[1]->position, &c = _vertices[2]->position;
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
    vec2 uvs = ((1-alpha-beta)*_vertices[0]->texCoord
                + alpha*_vertices[1]->texCoord
                + beta*_vertices[2]->texCoord);
    
    // Reject if we have an alpha texture
    if (_mesh && _mesh->_alphaTexture) {
        if (_mesh->_alphaTexture->evaluateFloat(uvs) < 0.1f) {
            return false;
        }
    }
    
    // Compute smoothed normal
    normal = ((1-alpha-beta)*_vertices[0]->normal
              + alpha*_vertices[1]->normal
              + beta*_vertices[2]->normal);
    
    // Update the ray
    ray.tmax = t;
    
    // Fill in intersection informations
    intersection->t = t;
    intersection->point = ray(t);
    intersection->normal = normal;
    intersection->uv = uvs;
    
    // Interpolate tangents
    intersection->tangentU = ((1-alpha-beta)*_vertices[0]->tangentU
                              + alpha*_vertices[1]->tangentU
                              + beta*_vertices[2]->tangentU);
    intersection->tangentV = ((1-alpha-beta)*_vertices[0]->tangentV
                              + alpha*_vertices[1]->tangentV
                              + beta*_vertices[2]->tangentV);

    intersection->rayEpsilon = 1e-3f * t;
    return true;
}

bool Triangle::intersectP(const Ray& ray) const {
    const vec3 &a = _vertices[0]->position, &b = _vertices[1]->position, &c = _vertices[2]->position;
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
    vec2 uvs = ((1-alpha-beta)*_vertices[0]->texCoord
                + alpha*_vertices[1]->texCoord
                + beta*_vertices[2]->texCoord);
    
    // Reject if we have an alpha texture
    if (_mesh && _mesh->_alphaTexture) {
        if (_mesh->_alphaTexture->evaluateFloat(uvs) == 0.0f) {
            return false;
        }
    }
    
    return true;
}

AABB Triangle::getBoundingBox() const {
    return AABB::Union(AABB(_vertices[0]->position, _vertices[1]->position), _vertices[2]->position);
}