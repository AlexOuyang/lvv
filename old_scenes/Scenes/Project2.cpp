//
//  Project2.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/23/14.
//
//

#include "Utilities/Main.h"

void project2(Scene* &scene, Camera* &camera, QtFilm* &film) {
    // Create scene
    scene = new Scene(new ListAggregate());
    *scene << new SkyLight(vec3(0.8f, 0.8f, 1.0f));
    
    // Materials
    Matte* white = new Matte();
    white->setColor(vec3(0.3f, 0.3f, 0.35f));
    
    Matte* white2 = new Matte();
    white2->setColor(vec3(0.7f));
    
    // Create ground
    Mesh* groundShape = ShapesUtilities::CreateBox(5.0f, 0.1f, 5.0f);
    GeometricPrimitive* ground = new GeometricPrimitive(groundShape, white);
    
    // Create dragon
    AssimpImporter importer;
    importer.setDefaultMaterial(white2);
    
    Main::startClock("Importing model...");
    Aggregate* model = new BVHAccelerator();
    importer.importModel(model, "/Users/gael/Desktop/Courses/CSE_168/models/dragon.ply");
    Main::endClock("Model imported in");
    
    // Add ground to model aggregate (cannot be directly added to the scene)
    *model << std::shared_ptr<Primitive>(ground);
    
    Main::buildAccelerationStructures(model);
    
    *scene << model;
    
    TransformedPrimitive* inst;
    Transform t;
    
    t.rotate((float)M_PI, vec3(0.0f, 0.1f, 0.0f));
    t.setTranslation(vec3(-0.05f, 0.0f, -0.1f));
    inst = new TransformedPrimitive(std::shared_ptr<Primitive>(model), t);
    
    *scene << inst;
    
    // Create lights
    DirectionalLight* sunlgt = new DirectionalLight();
    sunlgt->setSpectrum(Spectrum(vec3(1.0f, 1.0f, 0.9f)));
    sunlgt->setIntensity(1.0f);
    sunlgt->setDirection(vec3(2.0f, -3.0f, -2.0f));
    *scene << sunlgt;
    
    PointLight* light;
    
    light = new PointLight();
    light->setSpectrum(Spectrum(vec3(1.f, 0.2f, 0.2f)));
    light->setIntensity(0.02f);
    light->setPosition(vec3(-0.2f, 0.2f, 0.2f));
    *scene << light;
    
    light = new PointLight();
    light->setSpectrum(Spectrum(vec3(0.2f, 0.2f, 1.0f)));
    light->setIntensity(0.02f);
    light->setPosition(vec3(0.1f, 0.1f, 0.3f));
    *scene << light;
    
    // Create camera
    PerspectiveCamera* perspectiveCamera = new PerspectiveCamera();
    
    perspectiveCamera->lookAt(vec3(-0.1f, 0.1f, 0.2f), vec3(-0.05f, 0.12f, 0.0f));
    
    film = new QtFilm(vec2(800.0, 600));
    perspectiveCamera->setFilm(film);
    
    perspectiveCamera->setVFov(40.0f);
    perspectiveCamera->setAspect(film->resolution.x/film->resolution.y);
    
    camera = perspectiveCamera;
}