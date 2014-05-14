//
//  Car.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/24/14.
//
//

#include "Utilities/Main.h"

void car(Scene* &scene, Camera* &camera, QtFilm* &film) {
    // Create scene
    scene = new Scene(new ListAggregate());
    *scene << new SkyLight(Spectrum(0xF0FAFF).getColor());

    // Import cornel box
    AssimpImporter importer;
    importer.setDefaultMaterial(Main::matte);
    
    importer.setMaterialCallback([] (const AssimpImporter::MaterialAttributes& attrs) {
        Material* material = nullptr;
        
        if (attrs.name == "Avent_FELGE2") {
            Metal* metal = new Metal();
            metal->setIndices(2.485f, 3.433f);
            metal->setColor(Spectrum(0x525252).getColor());
            metal->setRoughness(0.2f);
            material = metal;
        }
        else if (!Spectrum(attrs.transparency).isBlack()) {
            Glass* glass = new Glass();
            glass->setIndexIn(1.003f);
            glass->setIndexOut(1.003f);
            glass->setAbsorptionColor(attrs.transparency);
            glass->setAbsorptionCoeff(0.0f);
            glass->setRoughness(0.1f);
            material = glass;
        } else if (attrs.shadingMode == AssimpImporter::Phong) {
            Glossy* glossy = new Glossy();
            glossy->setIndexIn(1.1f);
            glossy->setIndexOut(1.003f);
            glossy->setColor(attrs.color);
            glossy->setRoughness(0.9f);
            material = glossy;
        } else {
            Matte* matte = new Matte();
            matte->setColor(attrs.color);
            material = matte;
        }
        
        return material;
    });
    
    importer.setLightsCallback([&] (const AssimpImporter::LightAttributes& attrs) {
        PointLight* light = new PointLight();
        light->setPosition(attrs.position);
        light->setIntensity(attrs.intensity);
        light->setSpectrum(Spectrum(attrs.color));
        *scene << light;
    });
    
    Aggregate* model = new BVHAccelerator();
    
    importer.importModel(model, "/Users/gael/Desktop/Courses/CSE_168/models/scenes/car.dae",
                         &camera);
    
    Primitive* lightPrimitive = model->findPrimitive("areaLight");
    TransformedPrimitive* lightTransformed = nullptr;
    GeometricPrimitive* lightGeometric = nullptr;
    Mesh* lightShape = nullptr;
    
    if ((lightTransformed = dynamic_cast<TransformedPrimitive*>(lightPrimitive))) {
        if ((lightGeometric
             = dynamic_cast<GeometricPrimitive*>(lightTransformed->getPrimitive())))  {
            lightShape = dynamic_cast<Mesh*>(lightGeometric->getShape());
        }
    }
    
    // Create area light using model light shape
    if (lightShape) {
        // Transform light so its triangles are in world space
        const Transform& t = lightTransformed->getTransform();
        
        AreaLight* areaLight = AreaLight::CreateFromMesh(lightShape, t);
        areaLight->setSpectrum(Spectrum(vec3(1.0f, 1.0f, 1.0f)));
        areaLight->setIntensity(170.0f);
        lightGeometric->setAreaLight(areaLight);
        *scene << areaLight;
    } else {
        DirectionalLight* light = new DirectionalLight();
        light->setSpectrum(Spectrum(vec3(1.0f, 1.0f, 1.0f)));
        light->setIntensity(2.0f);
        light->setDirection(vec3(2.0f, -3.0f, -2.0f));
        *scene << light;
    }
    
    // Build acceleration structures
    Main::buildAccelerationStructures(model);
    
    *scene << model;
    
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
    
    film = new QtFilm(vec2(1920.f, 1080.f)/5.0f);
    perspectiveCamera->setFilm(film);
    
    perspectiveCamera->setAspect(film->resolution.x/film->resolution.y);
    
    camera = perspectiveCamera;
}