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
    
//    IntTexture* skyTexture = (IntTexture*)ImageLoading::LoadImage("/Users/gael/Desktop/Courses/CSE_168/models/textures/grace.jpg");
//    skyTexture->setScaleFactor(vec3(1.0f));
//    SkyLight* sky = new SkyLight(skyTexture);
//    Transform t;
//    t.m = glm::rotate(t.m, radians(178.f), vec3(0.f, 1.f, 0.f));
//    sky->setTransform(t);
//    scene->lights.push_back(sky);
    *scene << new SkyLight(vec3(0.8f, 0.9f, 1.0f));

    // Import cornel box
    AssimpImporter importer;
    importer.setDefaultMaterial(Main::matte);
    
    importer.setMaterialCallback([&] (const AssimpImporter::MaterialAttributes& attrs) {
        Material* material = nullptr;
        
        qDebug() << attrs.name.c_str();
        if (attrs.name == "chess_blackSet1" || attrs.name == "chess_redSet1"
            || attrs.name == "chess_whiteSet1") {
            AshikhminMaterial* mtl = new AshikhminMaterial();
            
            if (!attrs.texturePath.empty()) {
                IntTexture* t = dynamic_cast<IntTexture*>(ImageLoading::LoadImage(attrs.texturePath));
                if (t) {
                    t->setScaleFactor(vec3(1.3f));
                    mtl->setDiffuseColor(t);
                }
            }
            if (attrs.name == "chess_redSet1") {
                mtl->setDiffuseColor(vec3(1.0f, 0.1f, 0.1f));
                mtl->setDiffuseIntensity(0.8f);
                mtl->setSpecularIntensity(0.2f);
                mtl->setSpecularColor(vec3(1.0f, 1.0f, 1.0f));
                mtl->setRoughness(1000.0f, 1000.0f);
            } else {
                mtl->setSpecularColor(vec3(1.0f, 1.0f, 1.0f));
                mtl->setDiffuseIntensity(0.9f);
                mtl->setSpecularIntensity(0.1f);
                mtl->setRoughness(1000.0f, 1000.0f);
            }
            material = mtl;
        }
        if (attrs.name == "water_material") {
            Glass* glass = new Glass();
            glass->setIndexIn(1.33f);
            glass->setIndexOut(1.0003f);
            glass->setRoughness(0.1f);
            material = glass;
        } else if (attrs.name == "Mach2_chrome1") {
            material = Main::steel;
        }
        
        return material;
    });
    
    importer.setLightsCallback([&] (const AssimpImporter::LightAttributes& attrs) {
        DirectionalLight* sunlgt = new DirectionalLight();
        sunlgt->setSpectrum(Spectrum(vec3(1.0f, 1.0f, 0.9f)));
        sunlgt->setIntensity(1.8f);
        sunlgt->setDirection(attrs.direction);
        *scene << sunlgt;
    });
    
    importer.setPrimitivesCallback([&] (TransformedPrimitive* tp, GeometricPrimitive* p) {
        const std::string& name = tp->getName();
        
        qDebug() << name;
        
        if (name == "EU50_1_EU50Leaf4") {
            Mesh* mesh = dynamic_cast<Mesh*>(p->getShape());
            if (mesh) {
                Texture* alphaTexture = ImageLoading::LoadFloatImage("/Users/gael/Downloads/EU50_Ostrya_carpinifolia_European_Hop_Hornbeam_maya/Models/EU50lef4_a.tif");
                mesh->setAlphaTexture(alphaTexture);
            }
        } else if (name.find("envmap") != std::string::npos) {
            return false;
        }
        
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
                return true;
            }
            
            // Transform light so its triangles are in world space
            const Transform& t = tp->getTransform();
            
            AreaLight* areaLight = AreaLight::CreateFromMesh(shape, t);
            areaLight->setSpectrum(Spectrum(1.5f));
            areaLight->setIntensity(10.0f);
            p->setAreaLight(areaLight);
            *scene << areaLight;
        }
        
        return true;
    });
    
    Aggregate* model = new BVHAccelerator();
    
    importer.importModel(model, "/Users/gael/Desktop/Courses/CSE_168/models/scenes/test.dae",
                         &camera);
    
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
//        perspectiveCamera->setApertureSize(0.04f);
//        perspectiveCamera->setFocusPoint(vec3(0.179f, 0.0f, 1.471f));
    }
    
    film = new QtFilm(vec2(1024.f, 768.f)/1.0f);
    perspectiveCamera->setFilm(film);
    
    perspectiveCamera->setAspect(film->resolution.x/film->resolution.y);
    
    camera = perspectiveCamera;
}