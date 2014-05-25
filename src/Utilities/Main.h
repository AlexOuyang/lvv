//
//  Main.h
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/5/14.
//
//

#ifndef __CSE168_Rendering__Main__
#define __CSE168_Rendering__Main__

#include <thread>
#include <QTimer>
#include <QTime>
#include <QDebug>

#include "Core/Core.h"
#include "Utilities/AssimpImporter.h"
#include "Core/Scene.h"
#include "Core/Renderer.h"
#include "Core/Camera.h"
#include "Films/QtFilm.h"
#include "Films/ImageFilm.h"
#include "Core/ListAggregate.h"
#include "Accelerators/BVHAccelerator.h"
#include "Lights/SkyLight.h"
#include "Lights/PointLight.h"
#include "Lights/DirectionalLight.h"
#include "Lights/AreaLight.h"
#include "Core/CameraSample.h"
#include "Core/Spectrum.h"
#include "Films/QtFilm.h"
#include "Core/GeometricPrimitive.h"
#include "Core/TransformedPrimitive.h"
#include "Shapes/ShapesUtilities.h"
#include "Shapes/Sphere.h"
#include "Shapes/Plane.h"
#include "Cameras/PerspectiveCamera.h"
#include "Materials/Matte.h"
#include "Materials/Metal.h"
#include "Materials/Glass.h"
#include "Materials/Glossy.h"
#include "Materials/AshikhminMaterial.h"
#include "Core/Texture.h"
#include "Utilities/ImageLoading.h"
#include "Volumes/HomogeneousVolume.h"
#include "Volumes/DensityVolume.h"

// Scenes
void project1(Scene* &scene, Camera* &camera, QtFilm* &film);
void randomSpheres(Scene* &scene, Camera* &camera, QtFilm* &film);
void project2(Scene* &scene, Camera* &camera, QtFilm* &film);
void project3(Scene* &scene, Camera* &camera, QtFilm* &film);
void instancing(Scene* &scene, Camera* &camera, QtFilm* &film);
void cornellBox(Scene* &scene, Camera* &camera, QtFilm* &film);
void materials(Scene* &scene, Camera* &camera, QtFilm* &film);
void glassScene(Scene* &scene, Camera* &camera, QtFilm* &film);
void car(Scene* &scene, Camera* &camera, QtFilm* &film);
void weapon(Scene* &scene, Camera* &camera, QtFilm* &film);
void colors(Scene* &scene, Camera* &camera, QtFilm* &film);
void atrium(Scene* &scene, Camera* &camera, QtFilm* &film);
void volume(Scene* &scene, Camera* &camera, QtFilm* &film);
void base(Scene* &scene, Camera* &camera, QtFilm* &film);

class Main : public QObject {
    Q_OBJECT
public:
    
    Main();
    ~Main();
    
    void renderThread();
    
    static void startClock(const std::string& message);
    static void endClock(const std::string& message);
    static QTime clock;
    static bool clockActive;
    
    static void buildAccelerationStructures(Aggregate* model);
    
    template<class T>
    static T findPrimitive(Aggregate* aggregate, const std::string& name) {
        Primitive* p = aggregate->findPrimitive(name);
        TransformedPrimitive* tp = dynamic_cast<TransformedPrimitive*>(p);
        if (!tp) {
            return nullptr;
        }
        return dynamic_cast<T>(tp->getPrimitive());
    }
    
    static Matte* matte;
    static Metal* gold;
    static Metal* steel;
    static Metal* copper;
    static Glass* glass;
    static Glossy* glossy;
    
    static void initMaterials();
    
public slots:
    void refresh();
    void toggleRendering();

private:
    Scene*      _scene;
    Renderer*   _renderer;
    Camera*     _camera;
    QtFilm*     _film;
    
    QTimer          _timer;
    std::thread*    _thread;
    bool            _continueRendering;
};

#endif /* defined(__CSE168_Rendering__Main__) */
