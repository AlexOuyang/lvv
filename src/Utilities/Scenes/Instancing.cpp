//
//  Instancing.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/23/14.
//
//

#include "Utilities/Main.h"

void instancing(Scene* &scene, Camera* &camera, QtFilm* &film) {
    // Create scene
    scene = new Scene(new ListAggregate());
    scene->lights.push_back(new SkyLight(Spectrum(0xE8FBFF)));
    
    // Materials
    Material* white = new Matte();
    
    Metal* gold = new Metal();
    gold->eta = 0.37f;
    gold->k = 2.82f;
    gold->color = Spectrum(vec3(255.0f, 215.f, 0.0f)/255.f);
    
    Metal* copper = new Metal();
    copper->eta = 0.617f;
    copper->k = 2.63f;
    copper->color = Spectrum(vec3(184.f, 115.f, 51.f)/255.f);
    
    Glass* glass = new Glass();
    glass->indexIn = 1.45f;
    glass->indexOut = 1.0003f;
    glass->absorptionColor = Spectrum(0xFF9500).getColor();
    glass->absorptionCoeff = 2.f;
    
    // Import model
    AssimpImporter importer;
    importer.setDefaultMaterial(white);
    
    Aggregate* aggregate = new BVHAccelerator();
    
    importer.importModel(aggregate, "/Users/gael/Desktop/Courses/CSE_168/models/dragon/dragon.ply");
    
    Primitive* p = aggregate->findPrimitive("Primitive 3");
    GeometricPrimitive* dragon = dynamic_cast<GeometricPrimitive*>(p);
    
    if (!p) {
        qDebug() << "Error: Cannot find model";
        abort();
    }
    
    dragon->setMaterial(glass);
    
    Main::startClock("Generating bvh...");
    
    aggregate->preprocess();
    
    Main::endClock("Model bvh generated in");
    
    
    BVHAccelerator* grid = new BVHAccelerator();
    
    for (int i = 0; i < 100; ++i) {
        for (int j = 0; j < 100; ++j) {
            TransformedPrimitive* instance;
            
            Transform t;
            t.m = glm::scale(t.m, vec3(1.0f, 1.0f, 1.0f)*20.f);
            t.m[3].x = -200.0f + ((float)i/100)*400.0f;
            t.m[3].z = 10-(((float)j/100)*800.0f);
            instance = new TransformedPrimitive(aggregate, t);
            
            *grid << instance;
        }
    }
    
    grid->preprocess();
    
    *scene->aggregate << grid;
    
    Plane* groundShape = new Plane();
    GeometricPrimitive* ground = new GeometricPrimitive(groundShape, white);
    *scene->aggregate << ground;
    
    // Create lights
    DirectionalLight* sunlgt = new DirectionalLight();
    sunlgt->setSpectrum(Spectrum(vec3(1.0f, 1.0f, 1.0f)));
    sunlgt->setIntensity(0.6f);
    sunlgt->setDirection(vec3(2.0f, -3.0f, -2.0f));
    scene->lights.push_back(sunlgt);
    
    sunlgt = new DirectionalLight();
    sunlgt->setSpectrum(Spectrum(vec3(1.0f, 1.0f, 1.0f)));
    sunlgt->setIntensity(0.6f);
    sunlgt->setDirection(vec3(-2.0f, -3.0f, -2.0f));
    scene->lights.push_back(sunlgt);
    
    // Create camera
    PerspectiveCamera* perspectiveCamera = new PerspectiveCamera();
    
    perspectiveCamera->lookAt(vec3(0.0f, 8.0f, 20.0f), vec3(0.0f, 1.0f, 0.0f));
    film = new QtFilm(vec2(1280.0, 720.0));
    perspectiveCamera->film = film;
    
    perspectiveCamera->setVFov(40.0f);
    perspectiveCamera->setAspect(film->resolution.x/film->resolution.y);
    
    camera = perspectiveCamera;
}