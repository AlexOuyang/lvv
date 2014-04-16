//
//  GeometricPrimitive.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/14/14.
//
//

#include "GeometricPrimitive.h"

GeometricPrimitive::GeometricPrimitive(Shape* shape, Material* material)
: _shape(shape), _material(material) {
    
}

GeometricPrimitive::~GeometricPrimitive() {
    
}

bool GeometricPrimitive::canIntersect() const {
    return _shape->canIntersect();
}

bool GeometricPrimitive::intersect(const Ray& ray, Intersection* intersection) const {
    if (!_shape->intersect(ray, intersection)) {
        return false;
    }
    intersection->material = _material;
    return true;
}

bool GeometricPrimitive::intersectP(const Ray& ray) const {
    return _shape->intersectP(ray);
}

AABB GeometricPrimitive::getBoundingBox() const {
    return _shape->getBoundingBox();
}

void GeometricPrimitive::refine(std::vector<Primitive*> &refined) const {
    std::vector<Shape*> refinedShapes;
    _shape->refine(refinedShapes);
    
    for (Shape* s : refinedShapes) {
        refined.push_back(new GeometricPrimitive(s, _material));
    }
}