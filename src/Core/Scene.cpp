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

#include "ListAggregate.h"
#include "accelerators/BVHAccelerator.h"
#include "Material.h"

std::shared_ptr<Scene> Scene::Load(const rapidjson::Value& value) {
    std::shared_ptr<Scene> scene;
    Aggregate* aggregate = nullptr;
    
    // Create aggregate
    if (value.HasMember("aggregate")) {
        std::string str = value["aggregate"].GetString();
        std::transform(str.begin(), str.end(), str.begin(), ::tolower);
        if (str == "list") {
            aggregate = new ListAggregate();
        } else if (str == "bvh") {
            aggregate = new BVHAccelerator();
        }
    }
    if (!aggregate) {
        aggregate = new ListAggregate();
    }
    
    // Create scene
    scene = std::make_shared<Scene>(aggregate);
    
    // Load materials
    if (value.HasMember("materials")) {
        const rapidjson::Value& materials = value["materials"];
        if (materials.IsArray()) {
            for (int i = 0, l = materials.Size(); i < l; ++i) {
                std::shared_ptr<Material> material = Material::Load(materials[i]);
                if (!material->getName().empty()) {
                    scene->addMaterial(material->getName(), material);
                }
            }
        }
    }
    
    return scene;
}

Scene::Scene(Aggregate* aggregate) : _lights(), _aggregate(aggregate), _volume(nullptr), _materials() {
    
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

void Scene::addPrimitive(const std::shared_ptr<Primitive>& primitive) {
    *_aggregate << primitive;
}

void Scene::setVolume(Volume* volume) {
    _volume = volume;
}

Scene& Scene::operator<<(Light* light) {
    addLight(light);
    return *this;
}

Scene& Scene::operator<<(const std::shared_ptr<Primitive>& primitive) {
    addPrimitive(primitive);
    return *this;
}

const std::vector<Light*>& Scene::getLights() const {
    return _lights;
}

Volume* Scene::getVolume() const {
    return _volume;
}

void Scene::addMaterial(const std::string& name, std::shared_ptr<Material> mtl) {
    _materials[name] = mtl;
}

std::shared_ptr<Material> Scene::getMaterial(const std::string& name) {
    return _materials[name];
}