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

Triangle::Triangle() : vertices(), mesh(nullptr) {
    
}

Triangle::Triangle(Vertex* a, Vertex* b, Vertex* c) : vertices(), mesh(nullptr) {
    vertices[0] = a;
    vertices[1] = b;
    vertices[2] = c;    
}

Triangle::~Triangle() {
    
}

bool Triangle::intersect(const Ray& ray, Intersection* intersection) const {
    const vec3 &a = vertices[0]->position, &b = vertices[1]->position, &c = vertices[2]->position;
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
    vec2 uvs = ((1-alpha-beta)*vertices[0]->texCoord
                + alpha*vertices[1]->texCoord
                + beta*vertices[2]->texCoord);
    
    // Reject if we have an alpha texture
    if (mesh && mesh->alphaTexture) {
        if (mesh->alphaTexture->evaluateFloat(uvs) == 0.0f) {
            return false;
        }
    }
    
    // Compute smoothed normal
    normal = ((1-alpha-beta)*vertices[0]->normal
              + alpha*vertices[1]->normal
              + beta*vertices[2]->normal);
    
    // Update the ray
    ray.tmax = t;
    
    // Fill in intersection informations
    intersection->t = t;
    intersection->point = ray(t);
    intersection->normal = normal;
    intersection->uv = uvs;
    
    // Generate tangents
    if (dot(vertices[0]->tangentU, vertices[0]->tangentU) > 0.f) {
        intersection->tangentU = ((1-alpha-beta)*vertices[0]->tangentU
                                  + alpha*vertices[1]->tangentU
                                  + beta*vertices[2]->tangentU);
        intersection->tangentV = cross(normal, intersection->tangentU);
    } else {
        if (abs(normal.y) > 1.0f-0.00001f) {
            intersection->tangentU = vec3(1.0f, 0.0f, 0.0f);
            intersection->tangentV = vec3(0.0f, 0.0f, 1.0f);
        } else {
            intersection->tangentU = normalize(cross(vec3(0.0f, 1.0f, 0.0f), normal));
            intersection->tangentV = cross(normal, intersection->tangentU);
        }
    }

    intersection->rayEpsilon = 1e-3f * t;
    return true;
}

bool Triangle::intersectP(const Ray& ray) const {
    const vec3 &a = vertices[0]->position, &b = vertices[1]->position, &c = vertices[2]->position;
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
    vec2 uvs = ((1-alpha-beta)*vertices[0]->texCoord
                + alpha*vertices[1]->texCoord
                + beta*vertices[2]->texCoord);
    
    // Reject if we have an alpha texture
    if (mesh && mesh->alphaTexture) {
        if (mesh->alphaTexture->evaluateFloat(uvs) == 0.0f) {
            return false;
        }
    }
    
    return true;
}

AABB Triangle::getBoundingBox() const {
    return AABB::Union(AABB(vertices[0]->position, vertices[1]->position), vertices[2]->position);
}