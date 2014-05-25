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
    *scene << new SkyLight(Spectrum(0xE8FBFF).getColor());
    
    // Import model
    AssimpImporter importer;
    importer.setDefaultMaterial(Main::matte);
    
    Aggregate* aggregate = new BVHAccelerator();
    
    importer.importModel(aggregate, "/Users/gael/Desktop/Courses/CSE_168/models/dragon.ply");
    
    std::shared_ptr<GeometricPrimitive> dragon = Main::findPrimitive<GeometricPrimitive>(aggregate, "Primitive 3");
    
    if (!dragon) {
        qDebug() << "Error: Cannot find model";
        abort();
    }
    
    dragon->setMaterial(Main::steel);
    
    Main::buildAccelerationStructures(aggregate);
    
    
    BVHAccelerator* grid = new BVHAccelerator();
    
    for (int i = 0; i < 100; ++i) {
        for (int j = 0; j < 100; ++j) {
            TransformedPrimitive* instance;
            
            Transform t;
            t.scale(vec3(1.0f, 1.0f, 1.0f)*20.f);
            t.setTranslation(vec3(-200.0f + ((float)i/100)*400.0f, 0.f, 10-(((float)j/100)*800.0f)));
            instance = new TransformedPrimitive(std::shared_ptr<Primitive>(aggregate), t);
            
            *grid << std::shared_ptr<Primitive>(instance);
        }
    }
    
    grid->preprocess();
    
    *scene << grid;
    
    std::shared_ptr<Plane> groundShape = std::make_shared<Plane>();
    GeometricPrimitive* ground = new GeometricPrimitive(groundShape, Main::matte);
    *scene << ground;
    
    // Create lights
    DirectionalLight* sunlgt = new DirectionalLight();
    sunlgt->setSpectrum(Spectrum(vec3(1.0f, 1.0f, 1.0f)));
    sunlgt->setIntensity(0.6f);
    sunlgt->setDirection(vec3(2.0f, -3.0f, -2.0f));
    *scene << sunlgt;
    
    sunlgt = new DirectionalLight();
    sunlgt->setSpectrum(Spectrum(vec3(1.0f, 1.0f, 1.0f)));
    sunlgt->setIntensity(0.6f);
    sunlgt->setDirection(vec3(-2.0f, -3.0f, -2.0f));
    *scene << sunlgt;
    
    // Create camera
    PerspectiveCamera* perspectiveCamera = new PerspectiveCamera();
    
    perspectiveCamera->lookAt(vec3(0.0f, 8.0f, 20.0f), vec3(0.0f, 1.0f, 0.0f));
    film = new QtFilm(vec2(1280.0, 720.0));
    perspectiveCamera->setFilm(film);
    
    perspectiveCamera->setVFov(40.0f);
    perspectiveCamera->setAspect(film->resolution.x/film->resolution.y);
    
    camera = perspectiveCamera;
}