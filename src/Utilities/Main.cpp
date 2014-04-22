//
//  Main.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/5/14.
//
//

#include "Main.h"

#include <QDebug>

#include "Core/ListAggregate.h"
#include "Accelerators/BVHAccelerator.h"
#include "Lights/SkyLight.h"
#include "Lights/PointLight.h"
#include "Lights/DirectionalLight.h"
#include "Core/CameraSample.h"
#include "Core/Spectrum.h"
#include "Films/QtFilm.h"
#include "Core/GeometricPrimitive.h"
#include "Core/TransformedPrimitive.h"
#include "Shapes/ShapesUtilities.h"
#include "Shapes/Sphere.h"
#include "Shapes/Plane.h"
#include "Cameras/PerspectiveCamera.h"
#include "Materials/MatteMaterial.h"

// Scenes
float RangeRand(float a, float b) {
    return a + ((float)rand()/RAND_MAX)*(b-a);
}

void project1(Scene* &scene, Renderer* &renderer, Camera* &camera, QtFilm* &film) {
    
    // Create scene
    scene = new Scene(new ListAggregate());
    scene->lights.push_back(new SkyLight(Spectrum(vec3(0.8f, 0.9f, 1.0f))));
    
    // Materials
    Material* white = new MatteMaterial();
    
    // Create boxes
    Mesh* boxShape;
    GeometricPrimitive* box;
    
    // Box 1
    boxShape = ShapesUtilities::CreateBox(5.0f, 0.1f, 5.0f);
    box = new GeometricPrimitive(boxShape, white);
    *scene->aggregate << box;
    
    // Box 2
    boxShape = ShapesUtilities::CreateBox(1.0f, 1.0f, 1.0f);
    box = new GeometricPrimitive(boxShape, white);
    
    // Instances
    TransformedPrimitive* inst;
    Transform t;
    t.m = rotate(t.m, 0.5f, vec3(1.0f, 0.0f, 0.0f));
    t.m[3].y = 1.0f;
    inst = new TransformedPrimitive(box, t);
    *scene->aggregate << inst;

    t = Transform();
    t.m = rotate(t.m, 1.0f, vec3(0.0f, 1.0f, 0.0f));
    t.m[3] = vec4(vec3(-1.0f, 0.0f, 1.0f), 1.0f);
    inst = new TransformedPrimitive(box, t);
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
    
    renderer = new Renderer();
}

void randomSpheres(Scene* &scene, Renderer* &renderer, Camera* &camera, QtFilm* &film) {
    // Create scene
    scene = new Scene(new ListAggregate());
    scene->lights.push_back(new SkyLight(Spectrum(vec3(0.8f, 0.8f, 1.0f))));
    
    // Materials
    Material* white = new MatteMaterial();
    
    // Create ground plane
    Plane* groundShape = new Plane();
    GeometricPrimitive* ground = new GeometricPrimitive(groundShape, white);
    *scene->aggregate << ground;
    
    // Create spheres
    for(int i=0; i<20; ++i) {
        Sphere* sphereShape = new Sphere();
        GeometricPrimitive* sphere = new GeometricPrimitive(sphereShape, white);
        float rad = RangeRand(0.25f,0.5f);
        vec3 pos(RangeRand(-5.0f,5.0f), rad, RangeRand(-5.0f,5.0f));
        sphereShape->setRadius(rad);
        sphereShape->setCenter(pos);
        *scene->aggregate << sphere;
    }
    
    // Create lights
    DirectionalLight* sunlgt = new DirectionalLight();
    sunlgt->setSpectrum(Spectrum(vec3(1.0f, 1.0f, 0.9f)));
    sunlgt->setIntensity(1.0f);
    sunlgt->setDirection(vec3(2.0f, -3.0f, -2.0f));
    scene->lights.push_back(sunlgt);
    
    // Create camera
    PerspectiveCamera* perspectiveCamera = new PerspectiveCamera();
    
    perspectiveCamera->lookAt(vec3(-0.75f,0.25f,5.0f), vec3(0.0f,0.5f,0.0f));
    film = new QtFilm(vec2(800, 600));
    perspectiveCamera->film = film;
    
    perspectiveCamera->setVFov(40.0f);
    perspectiveCamera->setAspect(1.33f);
    
    camera = perspectiveCamera;
    
    renderer = new Renderer();
}

void project2(Scene* &scene, Renderer* &renderer, Camera* &camera, QtFilm* &film) {
    // Create scene
    scene = new Scene(new ListAggregate());
    scene->lights.push_back(new SkyLight(Spectrum(vec3(0.8f, 0.8f, 1.0f))));
    
    // Materials
    Material* white = new MatteMaterial();
    
    // Create ground
    Mesh* groundShape = ShapesUtilities::CreateBox(5.0f, 0.1f, 5.0f);
    GeometricPrimitive* ground = new GeometricPrimitive(groundShape, white);
    
    // Create dragon
    AssimpImporter importer;
    importer.setDefaultMaterial(white);
    
    Aggregate* model = new BVHAccelerator();
    importer.importModel(model, "/Users/gael/Desktop/Courses/CSE_168/models/dragon/dragon.ply");
    
    // Add ground to model aggregate (cannot be directly added to the scene)
    *model << ground;
    
    Main::startClock("Generating model bvh...");
    
    model->preprocess();
    
    Main::endClock("Model bvh generated in");
    
    *scene->aggregate << model;
    
    TransformedPrimitive* inst;
    Transform t;
    
    t.m = glm::rotate(t.m, (float)M_PI, vec3(0.0f, 0.1f, 0.0f));
    t.m[3] = vec4(-0.05f, 0.0f, -0.1f, 1.0f);
    inst = new TransformedPrimitive(model, t);
    
    *scene->aggregate << inst;
    
    // Create lights
    DirectionalLight* sunlgt = new DirectionalLight();
    sunlgt->setSpectrum(Spectrum(vec3(1.0f, 1.0f, 0.9f)));
    sunlgt->setIntensity(1.0f);
    sunlgt->setDirection(vec3(2.0f, -3.0f, -2.0f));
    scene->lights.push_back(sunlgt);
    
    PointLight* light;
    
    light = new PointLight();
    light->setSpectrum(Spectrum(vec3(1.f, 0.2f, 0.2f)));
    light->setIntensity(0.02f);
    light->setPosition(vec3(-0.2f, 0.2f, 0.2f));
    scene->lights.push_back(light);
    
    light = new PointLight();
    light->setSpectrum(Spectrum(vec3(0.2f, 0.2f, 1.0f)));
    light->setIntensity(0.02f);
    light->setPosition(vec3(0.1f, 0.1f, 0.3f));
    scene->lights.push_back(light);
    
    // Create camera
    PerspectiveCamera* perspectiveCamera = new PerspectiveCamera();
    
    perspectiveCamera->lookAt(vec3(-0.1f, 0.1f, 0.2f), vec3(-0.05f, 0.12f, 0.0f));

    film = new QtFilm(vec2(800.0, 600));
    perspectiveCamera->film = film;
    
    perspectiveCamera->setVFov(40.0f);
    perspectiveCamera->setAspect(film->resolution.x/film->resolution.y);
    
    camera = perspectiveCamera;
    
    renderer = new Renderer();
}

void model(Scene* &scene, Renderer* &renderer, Camera* &camera, QtFilm* &film) {
    // Create scene
    scene = new Scene(new ListAggregate());
    scene->lights.push_back(new SkyLight(Spectrum(vec3(0.8f, 0.9f, 1.0f))));
    
    // Materials
    Material* white = new MatteMaterial();
    
    // Import model
    AssimpImporter importer;
    importer.setDefaultMaterial(white);
    
    Aggregate* aggregate = new BVHAccelerator();
    
    importer.importModel(aggregate, "/Users/gael/Desktop/Courses/CSE_168/models/teapot/teapot.obj");
    //importer.importModel(aggregate, "/Users/gael/Desktop/Courses/CSE_168/models/Bedroom_OBJ/Bedroom.obj");
    //importer.importModel(aggregate, "/Users/gael/Desktop/Courses/CSE_168/models/dragon/dragon.ply");
    
    aggregate->preprocess();
    
    BVHAccelerator* grid = new BVHAccelerator();
    
    for (int i = 0; i < 100; ++i) {
        for (int j = 0; j < 100; ++j) {
            TransformedPrimitive* instance;
            
            Transform t;
            t.m = glm::scale(t.m, vec3(1.0f, 1.0f, 1.0f)*0.05f);
            t.m[3].x = -100.0f + i*10.0f;
            t.m[3].z = -(j*10.0f);
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
    PointLight* light;
    
    light = new PointLight();
    light->setPosition(vec3(0.4, 0.75, 1));
    light->setSpectrum(Spectrum(vec3(0.8f, 0.9f, 1.0f)));
    light->setIntensity(1.0);
    //scene->lights.push_back(light);
    
    DirectionalLight* sunlgt = new DirectionalLight();
    sunlgt->setSpectrum(Spectrum(vec3(1.0f, 1.0f, 0.9f)));
    sunlgt->setIntensity(1.0f);
    sunlgt->setDirection(vec3(2.0f, -3.0f, -2.0f));
    scene->lights.push_back(sunlgt);

    DirectionalLight* sunlgt2 = new DirectionalLight();
    sunlgt2->setSpectrum(Spectrum(vec3(1.0f, 1.0f, 0.9f)));
    sunlgt2->setIntensity(0.3f);
    sunlgt2->setDirection(vec3(2.0f, 3.0f, -2.0f));
    //scene->lights.push_back(sunlgt2);
    
    // Create camera
    PerspectiveCamera* perspectiveCamera = new PerspectiveCamera();
    
    perspectiveCamera->lookAt(vec3(0.0f, 10.0f, 20.0f), vec3(0.0f, 5.0f, 0.0f));
    //perspectiveCamera->lookAt(vec3(0.0f, 0.1f, 0.2f), vec3(0, 0.12f, 0.0f));
    //perspectiveCamera->lookAt(vec3(49.155, 190.717, -107.093), vec3(22.78, 162.45, -69.4));
    film = new QtFilm(vec2(800.0, 450.0)/1.0f);
    perspectiveCamera->film = film;
    
    perspectiveCamera->setVFov(40.0f);
    perspectiveCamera->setAspect(film->resolution.x/film->resolution.y);
    
    camera = perspectiveCamera;
    
    renderer = new Renderer();
}

QTime Main::clock = QTime();

Main::Main() {
    // Setup timer
    _timer.setInterval(1000/24); // Refresh 24 times per second
    connect(&_timer, SIGNAL(timeout()), this, SLOT(refresh()));
    
    project2(_scene, _renderer, _camera, _film);
    
    _film->show();
    
    // Launch timer
    _timer.start();
    
    // Launch rendering thread
    _thread = new std::thread(&Main::renderThread, this);
}

Main::~Main() {
    if (_thread) {
        _thread->join();
        delete _thread;
    }
}

void Main::renderThread() {
    startClock("Begin rendering...");
    _renderer->render(*_scene, _camera);
    endClock("Rendering done in");
}

void Main::refresh() {
    _film->repaint();
    float elapsed = ((float)clock.elapsed()/1000);
    QString msg = "Ellapsed time: " + QString::number(elapsed) + "s";
    _film->statusBar()->showMessage(msg);
}

void Main::startClock(const std::string& message) {
    qDebug() << message.c_str();
    clock.start();
}

void Main::endClock(const std::string& message) {
    float elapsed = ((float)clock.elapsed()/1000);
    qDebug() << message.c_str() << elapsed << "s";
}