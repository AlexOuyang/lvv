//
//  Main.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/5/14.
//
//

#include "Main.h"

#include <unistd.h>

QTime Main::clock = QTime();
bool Main::clockActive = false;

Matte* Main::matte = nullptr;
Metal* Main::gold = nullptr;
Metal* Main::steel = nullptr;
Metal* Main::copper = nullptr;
Glass* Main::glass = nullptr;
Glossy* Main::glossy = nullptr;

void Main::initMaterials() {
    // Materials
    Main::matte = new Matte();
    
    Main::gold = new Metal();
    Main::gold->setIndices(0.37f, 2.82f);
    Main::gold->setColor(vec3(212.f, 175.f, 55.f)/255.f);
    Main::gold->setRoughness(0.2f);
    
    Main::steel = new Metal();
    Main::steel->setIndices(2.485f, 3.433f);
    Main::steel->setColor(Spectrum(0xF0F0F0).getColor());
    Main::steel->setRoughness(0.2f);
    
    Main::copper = new Metal();
    Main::copper->setIndices(0.617f, 2.63f);
    Main::copper->setColor(vec3(184.f, 115.f, 51.f)/255.f);
    Main::copper->setRoughness(0.2f);
    
    Main::glass = new Glass();
    Main::glass->setIndexIn(1.33f);
    Main::glass->setIndexOut(1.0003f);
    Main::glass->setAbsorptionColor(Spectrum(0x305C8C).getColor());
    Main::glass->setAbsorptionCoeff(5.0f);
    Main::glass->setRoughness(0.2f);
    
    Main::glossy = new Glossy();
    Main::glossy->setColor(Spectrum(0.f).getColor());
    Main::glossy->setIndexIn(2.3f);
    Main::glossy->setIndexOut(1.0003f);
    Main::glossy->setRoughness(0.2f);
}

Main::Main() :
_scene(nullptr), _renderer(nullptr), _camera(nullptr), _film(nullptr),
_timer(), _thread(nullptr), _continueRendering(true) {
    
    // Setup timer
    _timer.setInterval(1000/24); // Refresh 24 times per second
    connect(&_timer, SIGNAL(timeout()), this, SLOT(refresh()));
    
    initMaterials();
    
    cornellBox(_scene, _camera, _film);
    
    RenderOptions options;
    options.maxThreadsCount = -1;
    options.maxRayDepth = 5;
    options.antialiasingSampling.count = 1;
    options.antialiasingSampling.jittered = true;
    _renderer = new Renderer(options);
//    connect(_film, SIGNAL(toggleRendering()), this, SLOT(toggleRendering()));
//    
//    _film->show();
//    
//    // Launch timer
//    _timer.start();
//    
//    // Launch rendering thread
//    _thread = new std::thread(&Main::renderThread, this);
}

Main::~Main() {
    if (_thread) {
        _thread->join();
        delete _thread;
    }
}

void Main::renderThread() {
    startClock("Begin rendering...");
    int sampleCount = 0;
    QTime sampleClock;
    float avgSampleTime = 0.f;
    while (true) {
        if (_continueRendering) {
            sampleClock.restart();
            _renderer->render(*_scene, _camera);
            float ellapsed = ((float)sampleClock.elapsed()/1000.f);
            ++sampleCount;
            avgSampleTime = (sampleCount > 1)
            ? ((avgSampleTime*(sampleCount-1))+ellapsed)/sampleCount : ellapsed;
            qDebug() << "Rendered sample" << sampleCount
            << "in" << ellapsed << "s, avg"
            << avgSampleTime << "s";
//            startClock("Applying filters");
//            if (sampleCount%20 == 0)
//                _film->applyFilters();
//            endClock("Filters applied in ");
        } else {
            usleep(10*1000);
        }
    }
}

void Main::refresh() {
    _film->repaint();
    float elapsed = ((float)clock.elapsed()/1000);
    if (clockActive) {
        QString msg = "Ellapsed time: " + QString::number(elapsed) + "s";
        _film->statusBar()->showMessage(msg);
    }
}

void Main::toggleRendering() {
    _continueRendering = !_continueRendering;
    if (_continueRendering) {
        startClock("Rendering restarted...");
    } else {
        endClock("Rendering stopped...");
    }
}

void Main::startClock(const std::string& message) {
    qDebug() << message.c_str();
    clock.restart();
    clockActive = true;
}

void Main::endClock(const std::string& message) {
    float elapsed = ((float)clock.elapsed()/1000);
    qDebug() << message.c_str() << elapsed << "s";
    clockActive = false;
}

void Main::buildAccelerationStructures(Aggregate *model) {
    Main::startClock("Generating bvh...");
    
    for (Primitive* p : model->getPrimitives()) {
        TransformedPrimitive* transformed = dynamic_cast<TransformedPrimitive*>(p);
        if (transformed && !transformed->getPrimitive()->canIntersect()) {
            Aggregate* accelerator = new BVHAccelerator();
            *accelerator << transformed->getPrimitive();
            accelerator->preprocess();
            transformed->setPrimitive(accelerator);
        }
    }
    
    model->preprocess();
    
    Main::endClock("Model bvh generated in");
}