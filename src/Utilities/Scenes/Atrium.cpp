//
//  Atrium.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/24/14.
//
//

#include "Utilities/Main.h"

void atrium(Scene* &scene, Camera* &camera, QtFilm* &film) {
    // Create scene
    scene = new Scene(new ListAggregate());
    
    *scene << new SkyLight(vec3(0.8f, 0.9f, 1.0f)*2.f);

    // Import cornel box
    AssimpImporter importer;
    importer.setDefaultMaterial(Main::matte);
    
    importer.setLightsCallback([&] (const AssimpImporter::LightAttributes& attrs) {
        DirectionalLight* sunlgt = new DirectionalLight();
        sunlgt->setSpectrum(Spectrum(vec3(1.0f, 1.0f, 0.9f)));
        sunlgt->setIntensity(2.0f);
        sunlgt->setDirection(attrs.direction);
        *scene << sunlgt;
    });
    
    importer.setPrimitivesCallback([&] (TransformedPrimitive* tp, GeometricPrimitive* p) {
        const std::string& name = tp->getName();
        
        //qDebug() << name;
        if (name == "envmap") {
            return false;
        }

        return true;
    });
    
    Aggregate* model = new BVHAccelerator();
    
    importer.importModel(model, "/Users/gael/Desktop/Courses/CSE_168/models/scenes/atrium.dae",
                         &camera);
    
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
        perspectiveCamera->setApertureSize(0.01f);
        perspectiveCamera->setFocusPoint(vec3(-3.08f, 1.59f, -1.310f));
    }
    
    film = new QtFilm(vec2(1024.f, 768.f)/1.5f);
    perspectiveCamera->setFilm(film);
    
    perspectiveCamera->setAspect(film->resolution.x/film->resolution.y);
    
    camera = perspectiveCamera;
}