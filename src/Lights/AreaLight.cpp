//
//  AreaLight.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/23/14.
//
//

#include "AreaLight.h"

#include <QDebug>

AreaLight* AreaLight::CreateFromMesh(Mesh* mesh, const Transform& t, bool inverseNormal, int indexOffset) {
    if (mesh->getTrianglesCount() != 2) {
        return nullptr;
    }
    AreaLight* light = new AreaLight();
    Triangle* triangles = mesh->getTriangles();
    vec3
        p1 = t(triangles[0].getVertex((0+indexOffset)%3)->position),
        p2 = t(triangles[0].getVertex((1+indexOffset)%3)->position),
        p3 = t(triangles[0].getVertex((2+indexOffset)%3)->position)
    ;
    light->setPoints(p1, p2, p3);
    vec3 normal = normalize(cross(p2 - p1, p3 - p1));
    if (inverseNormal) {
        normal = -normal;
    }
    light->setNormal(normal);
    return light;
}

AreaLight::AreaLight() : Light(),
_points(), _normal(),
_intensity(1.0f), _color(new UniformVec3Texture()) {
}

AreaLight::~AreaLight() {
    
}

void AreaLight::setPoints(const vec3& p1, const vec3& p2, const vec3& p3) {
    _points[0] = p1;
    _points[1] = p2;
    _points[2] = p3;
}

void AreaLight::setNormal(const vec3 &normal) {
    _normal = normal;
}

void AreaLight::setIntensity(float intensity) {
    _intensity = intensity * M_PI;
}

void AreaLight::setColor(const vec3 &color) {
    UniformVec3Texture* texture = dynamic_cast<UniformVec3Texture*>(_color);
    if (texture) {
        texture->setValue(color);
    }
}

void AreaLight::setColor(Texture* texture) {
    _color = texture;
}

Spectrum AreaLight::le(const Ray& ray, const Intersection* intersection) const {
    if (ray.tmax == INFINITY) {
        return Spectrum(0.f);
    }
    vec2 uv;
    if (intersection) {
        uv = intersection->uv;
    }
    return Spectrum(_color->evaluateVec3(uv) * _intensity);
}

Spectrum AreaLight::sampleL(const vec3 &point, float rayEpsilon,
                            const LightSample& lightSample,
                            vec3 *wi, VisibilityTester *vt) const {
//    // Sample random position over mesh
//    // Assuming all mesh triangles have the same probability
//    const Triangle& triangle = _mesh->_triangles[rand() % _mesh->_trianglesCount];
//    
//    // Sample point in triangle
//    float alpha = sqrt(lightSample.u)*lightSample.v;
//    float beta = 1.0f-sqrt(lightSample.u);
//    
//    vec3 sampledPosition = (
//                            triangle.getVertex(0)->position
//                            + alpha * (triangle.getVertex(1)->position
//                                       - triangle.getVertex(0)->position)
//                            + beta * (triangle.getVertex(2)->position
//                                         - triangle.getVertex(0)->position)
//                            );
//    vec3 normal = triangle.getVertex(0)->normal;
    
    vec3 v1 = _points[1] - _points[0], v2 = _points[2] - _points[0];
    vec3 sampledPosition = _points[0] + lightSample.u * v1 + lightSample.v * v2;
    
    vec3 dist = sampledPosition - point;
    float distLength = glm::length(dist);
    dist = dist / distLength;
    vec3 color = _color->evaluateVec3(vec2(lightSample.u, lightSample.v));

    *wi = dist;
    
    vt->setSegment(point, rayEpsilon, sampledPosition);
    return Spectrum(
            (color * _intensity)
            * dot(dist, _normal)
            * (1.0f / (distLength*distLength)));
}