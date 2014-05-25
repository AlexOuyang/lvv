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
    gold->setIndices(0.37f, 2.82f);
    gold->setColor(vec3(212.f, 175.f, 55.f)/255.f);
    gold->setRoughness(0.2);
    
    Metal* copper = new Metal();
    copper->setIndices(0.617f, 2.63f);
    copper->setColor(vec3(184.f, 115.f, 51.f)/255.f);
    copper->setRoughness(0.2f);
    
    Glass* glass = new Glass();
    glass->setIndexIn(1.45f);
    glass->setIndexOut(1.0003f);
    glass->setAbsorptionColor(Spectrum(0x2C7D18).getColor());
    glass->setAbsorptionCoeff(0.0f);
    glass->setRoughness(0.2f);
    
    Glossy* glossy = new Glossy();
    glossy->setColor(Spectrum(0xC20000).getColor());
    glossy->setIndexIn(2.3f);
    glossy->setIndexOut(1.0003f);
    glossy->setRoughness(0.2f);
    
    // Import cornel box
    AssimpImporter importer;
    importer.setDefaultMaterial(white);
    
    Aggregate* model = new BVHAccelerator();
    
    importer.importModel(model, "/Users/gael/Desktop/Courses/CSE_168/models/cornell-box/CornellBox-Sphere.obj");
    
    // Import dragon
    Aggregate* dragonAggregate = nullptr;
    if (false) {
        model->removePrimitive("leftSphere");
        model->removePrimitive("rightSphere");
        
        dragonAggregate = new BVHAccelerator();
        importer.importModel(dragonAggregate, "/Users/gael/Desktop/Courses/CSE_168/models/dragon.ply");
        
        std::shared_ptr<Primitive> p = dragonAggregate->findPrimitive("Primitive 20");
        TransformedPrimitive* dragonInstance = dynamic_cast<TransformedPrimitive*>(p.get());
        GeometricPrimitive* dragon = nullptr;
        
        if (dragonInstance) {
            dragon = dynamic_cast<GeometricPrimitive*>(dragonInstance->getPrimitive().get());
        }
        
        if (!dragon) {
            qDebug() << "Error: Cannot find dragon model";
            abort();
        }
        
        dragon->setMaterial(glass);
        
        Transform t;
        t.scale(vec3(7.0f));
        t.translate(vec3(0.0f, -0.03f, 0.0f));
        dragonInstance->setTransform(t);
        
        *scene << dragonInstance;
    } else {
        // Spheres materials
        std::shared_ptr<GeometricPrimitive> leftSphere = Main::findPrimitive<GeometricPrimitive>(model, "leftSphere");
        std::shared_ptr<GeometricPrimitive> rightSphere = Main::findPrimitive<GeometricPrimitive>(model, "rightSphere");
        if (!leftSphere || !rightSphere) {
            qDebug() << "Error: cannot find spheres in scene";
            abort();
        }
        
        leftSphere->setMaterial(gold);
        rightSphere->setMaterial(glass);
    }
    
    std::shared_ptr<Primitive> lightPrimitive = model->findPrimitive("light");
    TransformedPrimitive* lightInstance = dynamic_cast<TransformedPrimitive*>(lightPrimitive.get());
    GeometricPrimitive* lightGeometric = nullptr;
    std::shared_ptr<Mesh> lightShape = nullptr;
    
    if (lightInstance) {
        if ((lightGeometric = dynamic_cast<GeometricPrimitive*>(lightInstance->getPrimitive().get()))) {
            lightShape = std::dynamic_pointer_cast<Mesh>(lightGeometric->getShape());
        }
    }

    // Create area light using model light shape
    if (lightShape) {
        AreaLight* areaLight = AreaLight::CreateFromMesh(lightShape, Transform(), true, 1);
        areaLight->setColor(vec3(1.0f, 1.0f, 1.0f));
        areaLight->setIntensity(3.0f);
        lightGeometric->setAreaLight(areaLight);
        *scene << areaLight;
    } else {
        PointLight* pointLight = new PointLight();
        pointLight->setSpectrum(Spectrum(vec3(1.0f, 1.0f, 1.0f)));
        pointLight->setIntensity(2.0f);
        pointLight->setPosition(vec3(0.2300, 1.5700, -0.2200));
        *scene << pointLight;
    }
    
    // Build acceleration structure
    Main::buildAccelerationStructures(model);
    if (dragonAggregate) {
        Main::buildAccelerationStructures(dragonAggregate);
    }
    
    // Create volume
//    HomogeneousVolume* volume = new HomogeneousVolume(model->getBoundingBox());
//    volume->setSigmaA(Spectrum(vec3(0.1f)));
//    volume->setSigmaS(Spectrum(vec3(0.5f)));
//    volume->setLe(Spectrum(0.1f));
//    
//    scene->setVolume(volume);
    
    *scene << model;
    
    // Create camera
    PerspectiveCamera* perspectiveCamera = new PerspectiveCamera();
    
    perspectiveCamera->lookAt(vec3(0.0f, 0.75f, 3.0f), vec3(0.0f, 0.75f, 0.0f));
    film = new QtFilm(vec2(800.f, 600.f)/1.0f);
    perspectiveCamera->setFilm(film);
    
    perspectiveCamera->setVFov(40.0f);
    perspectiveCamera->setAspect(film->resolution.x/film->resolution.y);
    
    camera = perspectiveCamera;
}