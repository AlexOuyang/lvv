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
    scene->lights.push_back(new SkyLight(Spectrum(0xF0FAFF)));
    
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
    glass->absorptionColor = Spectrum(0x305C8C).getColor();
    glass->absorptionCoeff = 5.0f;
    glass->roughness = 0.2f;
    
    Glossy* glossy = new Glossy();
    glossy->color = Spectrum(0.f);
    glossy->indexIn = 2.3f;
    glossy->indexOut = 1.0003f;
    glossy->roughness = 0.2f;

    // Import cornel box
    AssimpImporter importer;
    importer.setDefaultMaterial(white);
    
    Aggregate* model = new BVHAccelerator();
    
    importer.importModel(model, "/Users/gael/Desktop/scene.dae", &camera);
    
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
    buddha->setMaterial(gold);
    GeometricPrimitive* dragon = Main::findPrimitive<GeometricPrimitive*>(model, "dragon_body");
    dragon->setMaterial(glass);
    GeometricPrimitive* dragonStatue = Main::findPrimitive<GeometricPrimitive*>(model, "dragon_statue_body");
    dragonStatue->setMaterial(copper);
    
    // Stands
    GeometricPrimitive* stand = Main::findPrimitive<GeometricPrimitive*>(model, "dragon_stand");
    stand->setMaterial(glossy);
    stand = Main::findPrimitive<GeometricPrimitive*>(model, "buddha_stand");
    stand->setMaterial(glossy);
    stand = Main::findPrimitive<GeometricPrimitive*>(model, "dragon_statue_stand");
    stand->setMaterial(glossy);
    
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
    
    film = new QtFilm(vec2(2560.f, 1600.f)/1.0f);
    perspectiveCamera->film = film;
    
    //perspectiveCamera->setVFov(45.f);
    perspectiveCamera->setAspect(film->resolution.x/film->resolution.y);
    
    camera = perspectiveCamera;
}