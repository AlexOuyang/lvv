//
//  Glass.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/24/14.
//
//

#include "Utilities/Main.h"

void glassScene(Scene* &scene, Camera* &camera, QtFilm* &film) {
    // Create scene
    scene = new Scene(new ListAggregate());
    *scene << new SkyLight(Spectrum(0xF0FAFF).getColor());
    
    TransformedPrimitive* lightTransform = nullptr;
    GeometricPrimitive* lightGeometric = nullptr;
    Mesh* lightShape = nullptr;

    // Import cornel box
    AssimpImporter importer;
    importer.setDefaultMaterial(Main::matte);
    
    importer.setPrimitivesCallback([&] (TransformedPrimitive* tp, GeometricPrimitive* p) {
        const std::string& name = tp->getName();
        
        if (name == "glass") {
            p->setMaterial(Main::glass);
        } else if (name == "stand") {
            p->setMaterial(Main::glossy);
        } else if (name == "areaLight") {
            lightTransform = tp;
            lightGeometric = p;
            lightShape = dynamic_cast<Mesh*>(p->getShape());
        }
        return true;
    });
    
    Aggregate* model = new BVHAccelerator();
    
    importer.importModel(model, "/Users/gael/Desktop/Courses/CSE_168/models/scenes/glass.dae", &camera);
    
    // Create area light using model light shape
    if (lightShape) {
        // Transform light so its triangles are in world space
        const Transform& t = lightTransform->getTransform();
        
        AreaLight* areaLight = AreaLight::CreateFromMesh(lightShape, t);
        areaLight->setSpectrum(Spectrum(vec3(1.0f, 1.0f, 1.0f)));
        areaLight->setIntensity(70.0f);
        lightGeometric->setAreaLight(areaLight);
        *scene << areaLight;
    } else {
        DirectionalLight* light = new DirectionalLight();
        light->setSpectrum(Spectrum(vec3(1.0f, 1.0f, 1.0f)));
        light->setIntensity(2.0f);
        light->setDirection(vec3(2.0f, -3.0f, -2.0f));
        *scene << light;
    }
    
    // Build acceleration structures
    Main::buildAccelerationStructures(model);
    
    *scene << model;
    
    // Create camera
    PerspectiveCamera* perspectiveCamera = nullptr;
    if (!camera) {
        perspectiveCamera = new PerspectiveCamera();
        
        perspectiveCamera->lookAt(vec3(-0.5f, 3.7f, 10.58f), vec3(-0.5f, 3.7f, 9.58f));
    } else {
        perspectiveCamera = dynamic_cast<PerspectiveCamera*>(camera);
        if (!perspectiveCamera) {
            qDebug() << "Wrong camera type";
            abort();
        }
    }
    
    film = new QtFilm(vec2(1280.f, 1024.f)/1.0f);
    perspectiveCamera->setFilm(film);
    
    perspectiveCamera->setAspect(film->resolution.x/film->resolution.y);
    
    camera = perspectiveCamera;
}