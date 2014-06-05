//
//  GeometricPrimitive.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/14/14.
//
//

#include "GeometricPrimitive.h"

GeometricPrimitive::GeometricPrimitive(const std::shared_ptr<Shape>& shape,
                                       const std::shared_ptr<Material>& material, AreaLight* areaLight)
: _shape(shape), _material(material), _areaLight(areaLight) {
    
}

GeometricPrimitive::~GeometricPrimitive() {
    
}

std::shared_ptr<Shape> GeometricPrimitive::getShape() const {
    return _shape;
}

void GeometricPrimitive::setMaterial(const std::shared_ptr<Material>& material) {
    _material = material;
}

bool GeometricPrimitive::canIntersect() const {
    return _shape->canIntersect();
}

bool GeometricPrimitive::intersect(const Ray& ray, Intersection* intersection) const {
    if (!_shape->intersect(ray, intersection)) {
        return false;
    }
    intersection->primitive = this;
    if (!_shape->hasMaterial()) {
        intersection->material = _material.get();
    }
    return true;
}

bool GeometricPrimitive::intersectP(const Ray& ray) const {
    return _shape->intersectP(ray);
}

AABB GeometricPrimitive::getBoundingBox() const {
    return _shape->getBoundingBox();
}

void GeometricPrimitive::refine(std::vector<std::shared_ptr<Primitive>> &refined) const {
    std::vector<std::shared_ptr<Shape>> refinedShapes;
    _shape->refine(refinedShapes);
    
    for (const std::shared_ptr<Shape>& s : refinedShapes) {
        refined.push_back(std::make_shared<GeometricPrimitive>(s, _material, _areaLight));
    }
}

void GeometricPrimitive::setAreaLight(AreaLight* areaLight) {
    _areaLight = areaLight;
}

AreaLight* GeometricPrimitive::getAreaLight() const {
    return _areaLight;
}