//
//  Project1.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/23/14.
//
//

#include "Utilities/Main.h"

void project1(Scene* &scene, Camera* &camera, QtFilm* &film) {
    // Create scene
    scene = new Scene(new ListAggregate());
    scene->lights.push_back(new SkyLight(Spectrum(vec3(0.8f, 0.9f, 1.0f))));
    
    // Materials
    Material* white = new Matte();
    
    // Create boxes
    Mesh* boxShape;
    GeometricPrimitive* box;
    Aggregate* aggregate = new BVHAccelerator();
    
    // Box 1
    boxShape = ShapesUtilities::CreateBox(5.0f, 0.1f, 5.0f);
    box = new GeometricPrimitive(boxShape, white);
    *aggregate << box;
    aggregate->preprocess();
    *scene->aggregate << aggregate;
    
    // Box 2
    boxShape = ShapesUtilities::CreateBox(1.0f, 1.0f, 1.0f);
    box = new GeometricPrimitive(boxShape, white);
    aggregate = new BVHAccelerator();
    *aggregate << box;
    aggregate->preprocess();
    
    // Instances
    TransformedPrimitive* inst;
    Transform t;
    t.m = rotate(t.m, 0.5f, vec3(1.0f, 0.0f, 0.0f));
    t.m[3].y = 1.0f;
    inst = new TransformedPrimitive(aggregate, t);
    *scene->aggregate << inst;
    
    t = Transform();
    t.m = rotate(t.m, 1.0f, vec3(0.0f, 1.0f, 0.0f));
    t.m[3] = vec4(vec3(-1.0f, 0.0f, 1.0f), 1.0f);
    inst = new TransformedPrimitive(aggregate, t);
    *scene->aggregate << inst;
    
    DirectionalLight* sunlgt = new DirectionalLight();
    sunlgt->setSpectrum(Spectrum(vec3(1.0f, 1.0f, 0.9f)));
    sunlgt->setIntensity(0.5f);
    sunlgt->setDirection(vec3(-0.5f, -1.0f, -0.5f));
    scene->lights.push_back(sunlgt);
    
    PointLight* redlgt = new PointLight();
    redlgt->setSpectrum(Spectrum(vec3(1.0f, 0.2f, 0.2f)));
    redlgt->setIntensity(2.0f);
    redlgt->setPosition(vec3(2.0f, 2.0f, 0.0f));
    scene->lights.push_back(redlgt);
    
    // Create camera
    PerspectiveCamera* perspectiveCamera = new PerspectiveCamera();
    
    perspectiveCamera->lookAt(vec3(2.0f, 2.0f, 5.0f), vec3(0.0f, 0.0f, 0.0f));
    film = new QtFilm(vec2(800, 600));
    perspectiveCamera->film = film;
    
    perspectiveCamera->setVFov(40.0f);
    perspectiveCamera->setAspect(1.33f);
    
    camera = perspectiveCamera;
}