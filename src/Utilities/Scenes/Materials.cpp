//
//  Materials.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/24/14.
//
//

#include "Utilities/Main.h"

void materials(Scene* &scene, Camera* &camera, QtFilm* &film) {
    // Create scene
    scene = new Scene(new ListAggregate());
    scene->lights.push_back(new SkyLight(Spectrum(0xF0FAFF).getColor()));

    // Import cornel box
    AssimpImporter importer;
    importer.setDefaultMaterial(Main::matte);
    
    Aggregate* model = new BVHAccelerator();
    
    Main::startClock("Loading model...");
    importer.importModel(model, "/Users/gael/Desktop/Courses/CSE_168/models/scenes/statues.dae", &camera);
    Main::endClock("Model loaded in");
    
    Primitive* lightPrimitive = model->findPrimitive("areaLight");
    TransformedPrimitive* lightTransformed = nullptr;
    GeometricPrimitive* lightGeometric = nullptr;
    Mesh* lightShape = nullptr;
    
    if ((lightTransformed = dynamic_cast<TransformedPrimitive*>(lightPrimitive))) {
        if ((lightGeometric
             = dynamic_cast<GeometricPrimitive*>(lightTransformed->getPrimitive())))  {
            lightShape = dynamic_cast<Mesh*>(lightGeometric->getShape());
        }
    }
    
    // Models
    GeometricPrimitive* buddha = Main::findPrimitive<GeometricPrimitive*>(model, "buddha_body");
    buddha->setMaterial(Main::gold);
    GeometricPrimitive* dragon = Main::findPrimitive<GeometricPrimitive*>(model, "dragon_body");
    dragon->setMaterial(Main::glass);
    GeometricPrimitive* dragonStatue = Main::findPrimitive<GeometricPrimitive*>(model, "dragon_statue_body");
    dragonStatue->setMaterial(Main::copper);
    
    // Stands
    GeometricPrimitive* stand = Main::findPrimitive<GeometricPrimitive*>(model, "dragon_stand");
    stand->setMaterial(Main::glossy);
    stand = Main::findPrimitive<GeometricPrimitive*>(model, "buddha_stand");
    stand->setMaterial(Main::glossy);
    stand = Main::findPrimitive<GeometricPrimitive*>(model, "dragon_statue_stand");
    stand->setMaterial(Main::glossy);
    
    // Create area light using model light shape
    if (lightShape) {
        // Transform light so its triangles are in world space
        const Transform& t = lightTransformed->getTransform();
        for (int i = 0; i < lightShape->verticesCount; ++i) {
            lightShape->vertices[i].position = t(lightShape->vertices[i].position);
            lightShape->vertices[i].normal = -t.applyToVector(lightShape->vertices[i].normal);
        }
        // Reset light transform to identity
        lightTransformed->setTransform(Transform());
        
        AreaLight* areaLight = new AreaLight(lightShape);
        areaLight->setSpectrum(Spectrum(vec3(1.0f, 1.0f, 1.0f)));
        areaLight->setIntensity(70.0f);
        lightGeometric->setAreaLight(areaLight);
        areaLight->samplingConfig.count = 2;
        scene->lights.push_back(areaLight);
    }
    
    // Build acceleration structures
    Main::buildAccelerationStructures(model);
    
    *scene->aggregate << model;
    
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
    
    film = new QtFilm(vec2(1920.f, 1080.f)*4.0f);
    perspectiveCamera->film = film;
    
    //perspectiveCamera->setVFov(45.f);
    perspectiveCamera->setAspect(film->resolution.x/film->resolution.y);
    
    camera = perspectiveCamera;
}