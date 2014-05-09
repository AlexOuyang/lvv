//
//  Base.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/24/14.
//
//

#include "Utilities/Main.h"

void base(Scene* &scene, Camera* &camera, QtFilm* &film) {
    // Create scene
    scene = new Scene(new ListAggregate());
    scene->lights.push_back(new SkyLight(Spectrum(0xF0FAFF)));

    // Import cornel box
    AssimpImporter importer;
    importer.setDefaultMaterial(Main::matte);
    
    Glass* glass = new Glass();
    glass->indexIn = 1.3f;
    glass->indexOut = 1.0003f;
    glass->absorptionColor = Spectrum(1.f).getColor();
    glass->absorptionCoeff = 5.0f;
    glass->roughness = 0.2f;
    
    Glossy* plastic = new Glossy();
    plastic->indexIn = 1.3f;
    plastic->indexOut = 1.003f;
    plastic->color = Spectrum(0xC7C7C7);
    
    Glossy* plastic2 = new Glossy();
    plastic2->indexIn = 2.3f;
    plastic2->indexOut = 1.003f;
    
    importer.setMaterialCallback([&] (const AssimpImporter::MaterialAttributes& attrs) {
        Material* material = nullptr;
        
        //qDebug() << attrs.name.c_str();
        if (attrs.name == "water_material") {
            Glass* glass = new Glass();
            glass->indexIn = 1.33f;
            glass->indexOut = 1.0003f;
            glass->roughness = 0.1f;
            material = glass;
        } else if (attrs.name == "Mach2_chrome1") {
            material = Main::steel;
        }
        
        return material;
    });
    
    importer.setLightsCallback([&] (const AssimpImporter::LightAttributes& attrs) {
        DirectionalLight* sunlgt = new DirectionalLight();
        sunlgt->setSpectrum(Spectrum(vec3(1.0f, 1.0f, 0.9f)));
        sunlgt->setIntensity(0.6f);
        sunlgt->setDirection(attrs.direction);
        scene->lights.push_back(sunlgt);
    });
    
    importer.setPrimitivesCallback([&] (TransformedPrimitive* tp, GeometricPrimitive* p) {
        const std::string& name = tp->name;
        
        qDebug() << name;
        if (name == "rotor") {
//            Transform t1 = tp->getTransform(), t2 = t1;
//            AnimatedTransform animated;
//            t2.m = glm::rotate(t2.m, radians(90.f), vec3(0.f, 1.f, 0.f));
//            animated.setTransforms(t1, t2);
//            tp->setTransform(animated);
//            Main::matte->setColor(vec3(1.f, 0.f, 0.f));
//            p->setMaterial(Main::matte);
        } else if (name.find("areaLight") != std::string::npos) {
            // Create area light
            Mesh* shape = dynamic_cast<Mesh*>(p->getShape());
            if (!shape) {
                return;
            }
            
            // Transform light so its triangles are in world space
            const Transform& t = tp->getTransform();
            for (int i = 0; i < shape->verticesCount; ++i) {
                shape->vertices[i].position = t(shape->vertices[i].position);
                shape->vertices[i].normal = t.applyToVector(shape->vertices[i].normal);
            }
            // Reset light transform to identity
            tp->setTransform(Transform());
            
            AreaLight* areaLight = new AreaLight(shape);
            areaLight->setSpectrum(Spectrum(1.0f));
            areaLight->setIntensity(5.0f);
            p->setAreaLight(areaLight);
            areaLight->samplingConfig.count = 1;
            scene->lights.push_back(areaLight);

        }
    });
    
    Aggregate* model = new BVHAccelerator();
    
    importer.importModel(model, "/Users/gael/Desktop/Courses/CSE_168/models/scenes/test.dae",
                         &camera);
    
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
    
    film = new QtFilm(vec2(1024.f, 768.f)/1.0f);
    perspectiveCamera->film = film;
    
    perspectiveCamera->setAspect(film->resolution.x/film->resolution.y);
    
    camera = perspectiveCamera;
}