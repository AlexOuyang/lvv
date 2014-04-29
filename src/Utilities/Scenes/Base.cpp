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
        
        qDebug() << attrs.name.c_str();
        if (attrs.name == "glass_material") {
            material = glass;
        } else if (attrs.name == "flatware_material") {
            material = Main::steel;
        } else if (attrs.name == "table_material") {
            material = Main::glossy;
        } else if (attrs.name == "plate_material") {
            material = plastic;
        } else if (attrs.name.find("sheet_holder") != std::string::npos) {
            plastic2->color = attrs.color;
            material = plastic2;
        }
        return material;
    });
    
    importer.setPrimitivesCallback([&] (TransformedPrimitive* tp, GeometricPrimitive* p) {
        const std::string& name = tp->name;
        
        if (name == "logo") {
            Main::glass->absorptionColor = Spectrum(0xAFF500).getColor();
            Main::glass->absorptionCoeff = 10.f;
            p->setMaterial(Main::glass);
        } else if (name.find("stand") != std::string::npos) {
            p->setMaterial(Main::glossy);
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
                shape->vertices[i].normal = -t.applyToVector(shape->vertices[i].normal);
            }
            // Reset light transform to identity
            tp->setTransform(Transform());
            
            AreaLight* areaLight = new AreaLight(shape);
            if (name == "areaLight1") {
                areaLight->setSpectrum(Spectrum(0xFFFBF2));
            } else {
                areaLight->setSpectrum(Spectrum(0xDBF7FF));
            }
            areaLight->setIntensity(20.0f);
            p->setAreaLight(areaLight);
            areaLight->samplingConfig.count = 6;
            scene->lights.push_back(areaLight);

        }
    });
    
    Aggregate* model = new BVHAccelerator();
    
    importer.importModel(model, "/Users/gael/Desktop/Courses/CSE_168/models/scenes/table.dae",
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
    
    film = new QtFilm(vec2(1920.f, 1080.f)/1.0f);
    perspectiveCamera->film = film;
    
    perspectiveCamera->setAspect(film->resolution.x/film->resolution.y);
    
    camera = perspectiveCamera;
}