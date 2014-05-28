//
//  main.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/3/14.
//
//

#include <QApplication>

#include "Utilities/Main.h"

#include "Core/ConfigFileReader.h"

int main(int argc, char* argv[]) {
    ConfigFileReader reader;
    
    if (!reader.readFile("/Users/gael/Desktop/Courses/CSE_168/scenes/cornell-box.json")) {
        return EXIT_FAILURE;
    }
    
    std::shared_ptr<Film> film = reader.getFilm();
    std::shared_ptr<Renderer> renderer = reader.getRenderer();
    std::shared_ptr<Scene> scene = reader.getScene();
    
    if (!film) {
        std::cerr << "Error: no film specified" << std::endl;
        return EXIT_FAILURE;
    }
    if (!renderer) {
        std::cerr << "Error: no renderer specified" << std::endl;
        return EXIT_FAILURE;
    }
    if (!scene) {
        std::cerr << "Error: no scene specified" << std::endl;
        return EXIT_FAILURE;
    }
    
    std::shared_ptr<Camera> camera = scene->getCamera();
    
    if (!camera) {
        std::cerr << "Error: no camera specified" << std::endl;
        return EXIT_FAILURE;
    }
    
    camera->setFilm(film);
    
    renderer->render(*scene, camera.get());
    
    std::shared_ptr<ImageFilm> image = std::dynamic_pointer_cast<ImageFilm>(film);
    if (image) {
        image->writeToFile();
    }
    
//    for (;;) {
//        std::shared_ptr<ImageFilm> film;
//        std::shared_ptr<Scene> scene;
//        std::shared_ptr<Camera> camera;
//        
//          // Create scene
//        scene = std::make_shared<Scene>(new ListAggregate());
//
//        // Materials
//        std::shared_ptr<Material> white = std::make_shared<Matte>();
//        
//        std::shared_ptr<Metal> gold = std::make_shared<Metal>();
//        gold->setIndices(0.37f, 2.82f);
//        gold->setColor(vec3(212.f, 175.f, 55.f)/255.f);
//        gold->setRoughness(0.2);
//        
//        std::shared_ptr<Glass> glass = std::make_shared<Glass>();
//        glass->setIndexIn(1.45f);
//        glass->setIndexOut(1.0003f);
//        glass->setAbsorptionColor(Spectrum(0x2C7D18).getColor());
//        glass->setAbsorptionCoeff(0.0f);
//        glass->setRoughness(0.2f);
//        
//        // Import cornel box
//        AssimpImporter importer;
//        importer.setDefaultMaterial(white);
//        
//        std::shared_ptr<Aggregate> model = std::make_shared<BVHAccelerator>();
//        
//        importer.importModel(model, "/Users/gael/Desktop/Courses/CSE_168/models/cornell-box/CornellBox-Sphere.obj", &camera);
//
//        // Spheres materials
//        std::shared_ptr<GeometricPrimitive> leftSphere = Main::findPrimitive<GeometricPrimitive>(model.get(), "leftSphere");
//        std::shared_ptr<GeometricPrimitive> rightSphere = Main::findPrimitive<GeometricPrimitive>(model.get(), "rightSphere");
//        if (!leftSphere || !rightSphere) {
//            qDebug() << "Error: cannot find spheres in scene";
//            abort();
//        }
//        
//        leftSphere->setMaterial(gold);
//        rightSphere->setMaterial(glass);
//            
//        std::shared_ptr<Primitive> lightPrimitive = model->findPrimitive("light");
//        std::shared_ptr<TransformedPrimitive> lightInstance = std::dynamic_pointer_cast<TransformedPrimitive>(lightPrimitive);
//        std::shared_ptr<GeometricPrimitive> lightGeometric;
//        std::shared_ptr<Mesh> lightShape;
//        
//        if (lightInstance) {
//            if ((lightGeometric = std::dynamic_pointer_cast<GeometricPrimitive>(lightInstance->getPrimitive()))) {
//                lightShape = std::dynamic_pointer_cast<Mesh>(lightGeometric->getShape());
//            }
//        }
//            
//        // Create area light using model light shape
//        if (lightShape) {
//            AreaLight* areaLight = AreaLight::CreateFromMesh(lightShape.get(), Transform(), true, 1);
//            areaLight->setColor(vec3(1.0f, 1.0f, 1.0f));
//            areaLight->setIntensity(3.0f);
//            lightGeometric->setAreaLight(areaLight);
//            *scene << areaLight;
//        }
//        
//        // Build acceleration structure
//        Main::buildAccelerationStructures(model.get());
//
//        *scene << model;
//        
//        // Create camera
//        std::shared_ptr<PerspectiveCamera> perspectiveCamera = std::make_shared<PerspectiveCamera>();
//        
//        perspectiveCamera->lookAt(vec3(0.0f, 0.75f, 3.0f), vec3(0.0f, 0.75f, 0.0f));
//        film = std::make_shared<ImageFilm>(vec2(800.f, 600.f)/2.0f);
//        perspectiveCamera->setFilm(film);
//        
//        perspectiveCamera->setVFov(40.0f);
//        perspectiveCamera->setAspect(film->resolution.x/film->resolution.y);
//        
//        camera = perspectiveCamera;
//        
//        // Create renderer
//        std::shared_ptr<Renderer> renderer = std::make_shared<Renderer>();
//
//        // Render image
//        renderer->render(*scene, camera.get());
//        
//        // Save rendered image
//        film->saveToFile("/Users/gael/Desktop/render.bmp");
//    }
//    return 0;
}