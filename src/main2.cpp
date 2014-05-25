//
//  main.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/3/14.
//
//

#include <QApplication>

#include "Utilities/Main.h"

int main(int argc, char* argv[]) {
    for (;;) {
        ImageFilm* film = new ImageFilm(vec2(800.f, 600.f));
        
        Scene* scene = nullptr;
        Camera* camera = nullptr;
        
          // Create scene
        scene = new Scene(new ListAggregate());

        // Materials
        Material* white = new Matte();
        
        Metal* gold = new Metal();
        gold->setIndices(0.37f, 2.82f);
        gold->setColor(vec3(212.f, 175.f, 55.f)/255.f);
        gold->setRoughness(0.2);
        
        Glass* glass = new Glass();
        glass->setIndexIn(1.45f);
        glass->setIndexOut(1.0003f);
        glass->setAbsorptionColor(Spectrum(0x2C7D18).getColor());
        glass->setAbsorptionCoeff(0.0f);
        glass->setRoughness(0.2f);
        
        // Import cornel box
        AssimpImporter importer;
        importer.setDefaultMaterial(white);
        
        Aggregate* model = new BVHAccelerator();
        
        importer.importModel(model, "/Users/gael/Desktop/Courses/CSE_168/models/cornell-box/CornellBox-Sphere.obj");

        // Spheres materials
        GeometricPrimitive* leftSphere = Main::findPrimitive<GeometricPrimitive*>(model, "leftSphere");
        GeometricPrimitive* rightSphere = Main::findPrimitive<GeometricPrimitive*>(model, "rightSphere");
        if (!leftSphere || !rightSphere) {
            qDebug() << "Error: cannot find spheres in scene";
            abort();
        }
        
        leftSphere->setMaterial(gold);
        rightSphere->setMaterial(glass);
            
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
            AreaLight* areaLight = AreaLight::CreateFromMesh(lightShape, Transform(), true, 1);
            areaLight->setColor(vec3(1.0f, 1.0f, 1.0f));
            areaLight->setIntensity(3.0f);
            lightGeometric->setAreaLight(areaLight);
            *scene << areaLight;
        }
        
        // Build acceleration structure
        Main::buildAccelerationStructures(model);
        
        *scene << model;
        
        // Create camera
        PerspectiveCamera* perspectiveCamera = new PerspectiveCamera();
        
        perspectiveCamera->lookAt(vec3(0.0f, 0.75f, 3.0f), vec3(0.0f, 0.75f, 0.0f));
        film = new ImageFilm(vec2(800.f, 600.f)/4.0f);
        perspectiveCamera->setFilm(film);
        
        perspectiveCamera->setVFov(40.0f);
        perspectiveCamera->setAspect(film->resolution.x/film->resolution.y);
        
        camera = perspectiveCamera;
        
        // Create renderer
        Renderer* renderer = new Renderer();
        
        // Render image
        //renderer->render(*scene, camera);
        
        // Save rendered image
        film->saveToFile("/Users/gael/Desktop/render.bmp");
        
        // Cleanup renderer
        delete renderer;
        
        // Cleanup scene
        delete scene;
        
        // Cleanup
        delete film;
    }
    return 0;
}