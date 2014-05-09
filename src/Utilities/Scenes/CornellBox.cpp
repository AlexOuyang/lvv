//
//  CornellBox.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/23/14.
//
//

#include "Utilities/Main.h"

void cornellBox(Scene* &scene, Camera* &camera, QtFilm* &film) {
    // Create scene
    scene = new Scene(new ListAggregate());
    //scene->lights.push_back(new SkyLight(Spectrum(vec3(1.0f))));
    
    // Materials
    Material* white = new Matte();
    
    Metal* gold = new Metal();
    gold->eta = 0.37f;
    gold->k = 2.82f;
    gold->color = Spectrum(vec3(212.f, 175.f, 55.f)/255.f);
    gold->roughness = 0.2;
    
    Metal* copper = new Metal();
    copper->eta = 0.617f;
    copper->k = 2.63f;
    copper->color = Spectrum(vec3(184.f, 115.f, 51.f)/255.f);
    copper->roughness = 0.2;
    
    Glass* glass = new Glass();
    glass->indexIn = 1.45f;
    glass->indexOut = 1.0003f;
    glass->absorptionColor = Spectrum(0x2C7D18).getColor();
    glass->absorptionCoeff = 5.0f;
    glass->roughness = 0.2f;
    
    Glossy* glossy = new Glossy();
    glossy->color = Spectrum(0xC20000);
    glossy->indexIn = 2.3f;
    glossy->indexOut = 1.0003f;
    glossy->roughness = 0.2f;
    
    // Import cornel box
    AssimpImporter importer;
    importer.setDefaultMaterial(white);
    
    Aggregate* model = new BVHAccelerator();
    
    importer.importModel(model, "/Users/gael/Desktop/Courses/CSE_168/models/cornell-box/CornellBox-Sphere.obj");
    
    // Import dragon
    Aggregate* dragonAggregate = nullptr;
    if (true) {
        model->removePrimitive("leftSphere");
        model->removePrimitive("rightSphere");
        
        dragonAggregate = new BVHAccelerator();
        importer.importModel(dragonAggregate, "/Users/gael/Desktop/Courses/CSE_168/models/dragon.ply");
        
        Primitive* p = dragonAggregate->findPrimitive("Primitive 20");
        TransformedPrimitive* dragonInstance = dynamic_cast<TransformedPrimitive*>(p);
        GeometricPrimitive* dragon = nullptr;
        
        if (dragonInstance) {
            dragon = dynamic_cast<GeometricPrimitive*>(dragonInstance->getPrimitive());
        }
        
        if (!dragon) {
            qDebug() << "Error: Cannot find dragon model";
            abort();
        }
        
        dragon->setMaterial(glass);
        
        Transform t;
        t.m = glm::scale(t.m, vec3(7.0f));
        t.m = glm::translate(t.m, vec3(0.0f, -0.03f, 0.0f));
        dragonInstance->setTransform(t);
        
        *scene->aggregate << dragonInstance;
    } else {
        // Spheres materials
        GeometricPrimitive* leftSphere = Main::findPrimitive<GeometricPrimitive*>(model, "leftSphere");
        GeometricPrimitive* rightSphere = Main::findPrimitive<GeometricPrimitive*>(model, "rightSphere");
        if (!leftSphere || !rightSphere) {
            qDebug() << "Error: cannot find spheres in scene";
            abort();
        }
        
        //leftSphere->setMaterial(gold);
        //rightSphere->setMaterial(glass);
    }
    
    Primitive* lightPrimitive = model->findPrimitive("light");
    TransformedPrimitive* lightInstance = dynamic_cast<TransformedPrimitive*>(lightPrimitive);
    GeometricPrimitive* lightGeometric = nullptr;
    Mesh* lightShape = nullptr;
    
    if (lightInstance) {
        if ((lightGeometric = dynamic_cast<GeometricPrimitive*>(lightInstance->getPrimitive()))) {
            lightShape = dynamic_cast<Mesh*>(lightGeometric->getShape());
        }
    }

    // Create area light using model light shape
    if (lightShape) {
        AreaLight* areaLight = new AreaLight(lightShape);
        areaLight->setSpectrum(Spectrum(vec3(1.0f, 1.0f, 1.0f)));
        areaLight->setIntensity(3.0f);
        lightGeometric->setAreaLight(areaLight);
        areaLight->samplingConfig.count = 1;
        scene->lights.push_back(areaLight);
    } else {
        PointLight* pointLight = new PointLight();
        pointLight->setSpectrum(Spectrum(vec3(1.0f, 1.0f, 1.0f)));
        pointLight->setIntensity(2.0f);
        pointLight->setPosition(vec3(0.2300, 1.5700, -0.2200));
        scene->lights.push_back(pointLight);
    }
    
    // Build acceleration structure
    Main::buildAccelerationStructures(model);
    if (dragonAggregate) {
        Main::buildAccelerationStructures(dragonAggregate);
    }
    
    *scene->aggregate << model;
    
    // Create camera
    PerspectiveCamera* perspectiveCamera = new PerspectiveCamera();
    
    perspectiveCamera->lookAt(vec3(0.0f, 0.75f, 3.0f), vec3(0.0f, 0.75f, 0.0f));
    film = new QtFilm(vec2(800.f, 600.f)/1.0f);
    perspectiveCamera->film = film;
    
    perspectiveCamera->setVFov(40.0f);
    perspectiveCamera->setAspect(film->resolution.x/film->resolution.y);
    
    camera = perspectiveCamera;
}