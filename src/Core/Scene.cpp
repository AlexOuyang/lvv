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

#include "SceneImporter.h"
#include "ListAggregate.h"
#include "accelerators/BVHAccelerator.h"
#include "Material.h"

Scene::Scene(Aggregate* aggregate) :
_lights(), _aggregate(aggregate), _volume(nullptr), _materials(), _cameras(), _defaultCamera() {
    
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

void Scene::preprocess() {
    _aggregate->preprocess();
}

void Scene::addLight(Light* light) {
    _lights.push_back(light);
}

void Scene::addPrimitive(const std::shared_ptr<Primitive>& primitive) {
    *_aggregate << primitive;
}

void Scene::setVolume(Volume* volume) {
    if (_volume) {
        delete _volume;
    }
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

Scene& Scene::operator<<(const std::shared_ptr<Camera>& camera) {
    addCamera(camera);
    return *this;
}

Scene& Scene::operator<<(Volume* volume) {
    setVolume(volume);
    return *this;
}

const Aggregate* Scene::getAggregate() const {
    return _aggregate;
}

const std::vector<Light*>& Scene::getLights() const {
    return _lights;
}

Volume* Scene::getVolume() const {
    return _volume;
}

void Scene::addMaterial(const std::shared_ptr<Material>& mtl) {
    const std::string& name = mtl->getName();
    
    if (_materials.find(name) == _materials.end()) {
        _materials[name] = mtl;
    }
}

std::shared_ptr<Material> Scene::getMaterial(const std::string& name) const {
    auto it = _materials.find(name);
    if (it != _materials.end()) {
        return it->second;
    }
    return std::shared_ptr<Material>();
}

void Scene::addCamera(const std::shared_ptr<Camera>& camera) {
    _cameras[camera->getName()] = camera;
}

std::shared_ptr<Camera> Scene::getCamera(const std::string& name) const {
    if (_cameras.size() == 0) {
        return std::shared_ptr<Camera>();
    }
    // Return first camera by default
    if (name.empty()) {
        return _cameras.begin()->second;
    }
    // Or retrieve camera for given name
    auto it = _cameras.find(name);
    if (it != _cameras.end()) {
        return it->second;
    }
    return std::shared_ptr<Camera>();
}

bool Scene::setDefaultCamera(const std::string& name) {
    _defaultCamera = name;
    return !!getCamera(name);
}

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
        } else {
            std::cerr << "Scene warning: unknown aggregate \"" << str << "\"" << std::endl;
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
                    scene->addMaterial(material);
                }
            }
        }
    }
    
    // Import scene from a file
    if (value.HasMember("import")) {
        std::shared_ptr<SceneImporter> importer = SceneImporter::Load(value["import"]);
        if (importer) {
            importer->import(*scene);
        }
    }
    
    // Load camera
    if (value.HasMember("camera")) {
        const rapidjson::Value& cameraValue = value["camera"];
        std::shared_ptr<Camera> camera;
        if (cameraValue.IsString()) { // Retrieve camera by name
            const std::string name = cameraValue.GetString();
            if (!scene->setDefaultCamera(name)) {
                std::cerr << "Scene error: camera not found \"" << name << "\"" << std::endl;
            }
        } else { // Create new camera
            camera = Camera::Load(cameraValue);
            scene->addCamera(camera);
        }
    }
    
    // Preprocess aggregate
    scene->preprocess();
    
    return scene;
}