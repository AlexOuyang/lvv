//
//  Glass.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/24/14.
//
//

#include "Utilities/Main.h"

void weapon(Scene* &scene, Camera* &camera, QtFilm* &film) {
    // Create scene
    scene = new Scene(new ListAggregate());
    scene->lights.push_back(new SkyLight(Spectrum(0xF0FAFF).getColor()));

    // Import cornel box
    AssimpImporter importer;
    importer.setDefaultMaterial(Main::matte);
    
    Aggregate* model = new BVHAccelerator();
    
    importer.importModel(model, "/Users/gael/Desktop/Courses/CSE_168/models/scenes/weapon.dae", &camera);
    
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
    
    // Materials
    Main::steel->color = Spectrum(0xB3B3B3);
    Main::glossy->color = Spectrum(0x00000);
    
    // Models
    GeometricPrimitive* body = Main::findPrimitive<GeometricPrimitive*>(model, "body");
    body->setMaterial(Main::gold);
    GeometricPrimitive* magazine = Main::findPrimitive<GeometricPrimitive*>(model, "magazine");
    magazine->setMaterial(Main::gold);
    GeometricPrimitive* bullet = Main::findPrimitive<GeometricPrimitive*>(model, "bullet_body_1");
    bullet->setMaterial(Main::steel);
    bullet = Main::findPrimitive<GeometricPrimitive*>(model, "bullet_body_2");
    bullet->setMaterial(Main::steel);
    bullet = Main::findPrimitive<GeometricPrimitive*>(model, "bullet_head_1");
    bullet->setMaterial(Main::copper);
    bullet = Main::findPrimitive<GeometricPrimitive*>(model, "bullet_head_2");
    bullet->setMaterial(Main::copper);
    
    // Stands
    GeometricPrimitive* stand = Main::findPrimitive<GeometricPrimitive*>(model, "stand");
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
        areaLight->setIntensity(30.0f);
        lightGeometric->setAreaLight(areaLight);
        areaLight->samplingConfig.count = 4;
        scene->lights.push_back(areaLight);
    } else {
        DirectionalLight* light = new DirectionalLight();
        light->setSpectrum(Spectrum(vec3(1.0f, 1.0f, 1.0f)));
        light->setIntensity(2.0f);
        light->setDirection(vec3(2.0f, -3.0f, -2.0f));
        scene->lights.push_back(light);
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
    
    film = new QtFilm(vec2(1280.f, 720.f)/1.0f);
    perspectiveCamera->film = film;
    
    //perspectiveCamera->setVFov(45.f);
    perspectiveCamera->setAspect(film->resolution.x/film->resolution.y);
    
    camera = perspectiveCamera;
}