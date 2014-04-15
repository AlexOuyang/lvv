//
//  Scene.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/3/14.
//
//

#include "Scene.h"

#include "Intersection.h"
#include "Ray.h"

Scene::Scene(Aggregate* aggregate) : aggregate(aggregate) {
    
}

Scene::~Scene() {
    
}

bool Scene::intersect(const Ray& ray, Intersection* intersection) const {
    return aggregate->intersect(ray, intersection);
}

bool Scene::intersectP(const Ray& ray) const {
    return aggregate->intersectP(ray);
}