//
//  Base.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/24/14.
//
//

#include "Utilities/Main.h"

void volume(Scene* &scene, Camera* &camera, QtFilm* &film) {
    // Create scene
    scene = new Scene(new ListAggregate());
    
//    *scene << new SkyLight(vec3(0.8f, 0.9f, 1.0f)*0.5f);

    // Import model
    AssimpImporter importer;
    importer.setDefaultMaterial(Main::matte);
    
    importer.setMaterialCallback([&] (const AssimpImporter::MaterialAttributes& attrs) {
        Material* material = nullptr;
        
        qDebug() << attrs.name.c_str();
        
        return material;
    });
    
    importer.setLightsCallback([&] (const AssimpImporter::LightAttributes& attrs) {
        if (attrs.type == AssimpImporter::DirectionalLight) {
            DirectionalLight* sunlgt = new DirectionalLight();
            sunlgt->setSpectrum(Spectrum(vec3(1.0f, 1.0f, 1.0f)));
            sunlgt->setIntensity(0.1f);
            sunlgt->setDirection(attrs.direction);
            *scene << sunlgt;
        } else if (attrs.type == AssimpImporter::PointLight) {
            PointLight* light = new PointLight();
            light->setSpectrum(Spectrum(attrs.color));
            light->setIntensity(80000.f);
            light->setPosition(attrs.position);
            *scene << light;
        }
    });
    
    AABB volumeBounds;
    importer.setPrimitivesCallback([&] (TransformedPrimitive* tp, GeometricPrimitive* p) {
        const std::string& name = tp->getName();
        
        qDebug() << name;
        if (name == "plane") {
            volumeBounds = p->getBoundingBox();
            volumeBounds = AABB::Union(volumeBounds, vec3(0.f, 14.f, 28.f));
        }
        if (name == "sphere") {
//            volumeBounds = tp->getBoundingBox();
//            Transform t;
//            t.translate(vec3(0.f, 2.f, 0.f));
//            t.scale(vec3(1.5f, 1.f, 1.f));
//            volumeBounds = t(volumeBounds);
//            return false;
            Mesh* mesh = dynamic_cast<Mesh*>(p->getShape());
            if (mesh) {
                Texture* alphaTexture = ImageLoading::LoadFloatImage("/Users/gael/Desktop/tex_metal_005_t.jpg");
                mesh->setAlphaTexture(alphaTexture);
            }
        }
        
        return true;
    });
    
    Aggregate* model = new BVHAccelerator();
    
    importer.importModel(model, "/Users/gael/Desktop/Courses/CSE_168/models/scenes/test.dae",
                         &camera);
    
    // Build acceleration structures
    Main::buildAccelerationStructures(model);
    
    *scene << model;

    // Create volume
    HomogeneousVolume* volume = new HomogeneousVolume(volumeBounds);
    volume->setSigmaA(Spectrum((Spectrum(1.f)-Spectrum(0x7EB5E6))*0.06f));
    volume->setSigmaS(Spectrum(vec3(0.15f)));
    
//    DensityVolume* volume = new DensityVolume(volumeBounds);
//    volume->setSigmaA(Spectrum(0.1f));
//    volume->setSigmaS(Spectrum(vec3(1.00f)));
//    volume->setLe(Spectrum(0.f));
    
    scene->setVolume(volume);
    
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
    
    film = new QtFilm(vec2(800.f, 600.f)/1.0f);
    perspectiveCamera->setFilm(film);
    
    perspectiveCamera->setAspect(film->resolution.x/film->resolution.y);
    
    camera = perspectiveCamera;
}