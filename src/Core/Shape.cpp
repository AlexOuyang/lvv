//
//  Shape.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/15/14.
//
//

#include "Shape.h"

Shape::~Shape() {
    
}

bool Shape::canIntersect() const {
    return true;
}

bool Shape::intersect(const Ray&, Intersection*) const {
    std::cerr << "Shape intersect not implemented" << std::endl;
    abort();
}

bool Shape::intersectP(const Ray&) const {
    std::cerr << "Shape intersectP not implemented" << std::endl;
    abort();
}

void Shape::refine(std::vector<std::shared_ptr<Shape>> &) const {
    std::cerr << "Shape refine not implemented" << std::endl;
    abort();
}

bool Shape::hasMaterial() const {
    return false;
}