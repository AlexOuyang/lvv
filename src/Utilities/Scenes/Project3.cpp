//
//  Project3.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 5/5/14.
//
//

#include "Utilities/Main.h"

void project3(Scene* &scene, Camera* &camera, QtFilm* &film) {
    // Create scene
    scene = new Scene(new ListAggregate());
    scene->lights.push_back(new SkyLight(vec3(0.8f, 0.9f, 1.0f)));
    
    // Materials
    const int nummtls = 4;
//    Material* mtl[nummtls];
//    Main::matte->setColor(vec3(1.0f)*0.7f);
//    mtl[0] = Main::matte;
//    Main::copper->color = vec3(0.9f, 0.6f, 0.5f);
//    mtl[1] = Main::copper;
//    Main::gold->color = vec3(0.95f, 0.7f, 0.3f);
//    mtl[2] = Main::gold;
//    Main::glossy->color = Spectrum(vec3(1.0f, 0.1f, 0.1f)*0.6f);
//    mtl[3] = Main::glossy;
    AshikhminMaterial* mtl[nummtls];
    
    // Diffuse
    mtl[0] = new AshikhminMaterial();
    mtl[0]->setSpecularIntensity(0.0f);
    mtl[0]->setDiffuseIntensity(1.0f);
    mtl[0]->setDiffuseColor(vec3(0.7f, 0.7f, 0.7f));
    
    // Roughened copper
    mtl[1] = new AshikhminMaterial();
    mtl[1]->setDiffuseIntensity(0.0f);
    mtl[1]->setSpecularIntensity(1.0f);
    mtl[1]->setSpecularColor(vec3(0.9f, 0.6f, 0.5f));
    mtl[1]->setRoughness(100.0f, 100.0f);
    
    // Anisotropic gold
    mtl[2] = new AshikhminMaterial();
    mtl[2]->setDiffuseIntensity(0.0f);
    mtl[2]->setSpecularIntensity(1.0f);
    mtl[2]->setSpecularColor(vec3(0.95f, 0.7f, 0.3f));
    mtl[2]->setRoughness(1.0f, 1000.0f);
    
    // Red plastic
    mtl[3] = new AshikhminMaterial();
    mtl[3]->setDiffuseColor(vec3(1.0f, 0.1f, 0.1f));
    mtl[3]->setDiffuseIntensity(0.8f);
    mtl[3]->setSpecularIntensity(0.2f);
    mtl[3]->setSpecularColor(vec3(1.0f, 1.0f, 1.0f));
    mtl[3]->setRoughness(1000.0f, 1000.0f);
    
    // Load dragon mesh
    AssimpImporter importer;
    importer.setDefaultMaterial(Main::matte);
    
    Main::startClock("Importing model...");
    Aggregate* dragon = new BVHAccelerator();
    importer.importModel(dragon, "/Users/gael/Desktop/Courses/CSE_168/models/dragon.ply");
    Main::endClock("Model imported in");
    
    // Create box tree
    Main::buildAccelerationStructures(dragon);
    
    Aggregate* aggregate = new BVHAccelerator();
    
    // Create dragon instances
    Transform t;
    for(int i = 0; i < nummtls; ++i) {
        t.m[3] = vec4(0.0f, 0.0f, -0.1f*float(i), 1.0f);
        TransformedPrimitive* inst;
        inst = new TransformedPrimitive(dragon, t);
        inst->setMaterial(mtl[i]);
        *aggregate << inst;
    }
    
    // Create ground
    Matte* lambert = new Matte();
    lambert->setColor(vec3(0.3f, 0.3f, 0.35f));
    Mesh* groundShape = ShapesUtilities::CreateBox(2.0f, 0.11f, 2.0f);
    GeometricPrimitive* ground = new GeometricPrimitive(groundShape, lambert);
    
    *aggregate << ground;
    
    // Add aggregate to scene
    aggregate->preprocess();
    *scene->aggregate << aggregate;
    
    // Create lights
    DirectionalLight* sunlgt = new DirectionalLight();
    sunlgt->setSpectrum(Spectrum(vec3(1.0f, 1.0f, 0.9f)));
    sunlgt->setIntensity(1.0f);
    sunlgt->setDirection(vec3(2.0f, -3.0f, -2.0f));
    scene->lights.push_back(sunlgt);
    
    // Create camera
    PerspectiveCamera* perspectiveCamera = new PerspectiveCamera();
    
    perspectiveCamera->lookAt(vec3(-0.5f, 0.25f, -0.2f), vec3(0.0f, 0.15f, -0.15f));
//    perspectiveCamera->lookAt(vec3(-0.2f, 0.12f, 0.25f), vec3(0.0f, 0.15f, -0.15f));
//    perspectiveCamera->setApertureSize(0.01f);
//    perspectiveCamera->setFocusDistance(0.29f);
    
    film = new QtFilm(vec2(800.0, 600));
    perspectiveCamera->film = film;
    
    perspectiveCamera->setVFov(40.0f);
    //perspectiveCamera->setAspect(film->resolution.x/film->resolution.y);
    perspectiveCamera->setAspect(1.33f);
    
    camera = perspectiveCamera;
}