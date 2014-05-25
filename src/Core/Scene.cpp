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

Scene::Scene(Aggregate* aggregate) : _lights(), _aggregate(aggregate), _volume(nullptr) {
    
}

Scene::~Scene() {
    // Cleanup
    if (_aggregate) {
        delete _aggregate;
    }
    if (_volume) {
        delete _volume;
    }
    for (Light* light : _lights) {
        delete light;
    }
}

bool Scene::intersect(const Ray& ray, Intersection* intersection) const {
    return _aggregate->intersect(ray, intersection);
}

bool Scene::intersectP(const Ray& ray) const {
    return _aggregate->intersectP(ray);
}

void Scene::addLight(Light* light) {
    _lights.push_back(light);
}

void Scene::addPrimitive(Primitive* primitive) {
    *_aggregate << primitive;
}

void Scene::setVolume(Volume* volume) {
    _volume = volume;
}

Scene& Scene::operator<<(Light* light) {
    addLight(light);
    return *this;
}

Scene& Scene::operator<<(Primitive* primitive) {
    addPrimitive(primitive);
    return *this;
}

const std::vector<Light*>& Scene::getLights() const {
    return _lights;
}

Volume* Scene::getVolume() const {
    return _volume;
}