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
    qDebug() << "Shape intersect not implemented";
    abort();
}

bool Shape::intersectP(const Ray&) const {
    qDebug() << "Shape intersectP not implemented";
    abort();
}

void Shape::refine(std::vector<std::shared_ptr<Shape>> &) const {
    qDebug() << "Shape refine not implemented";
    abort();
}