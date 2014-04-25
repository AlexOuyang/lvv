//
//  Main.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/5/14.
//
//

#include "Main.h"

QTime Main::clock = QTime();
bool Main::clockActive = false;

Main::Main() : _scene(nullptr), _renderer(nullptr), _camera(nullptr), _film(nullptr) {
    // Setup timer
    _timer.setInterval(1000/24); // Refresh 24 times per second
    connect(&_timer, SIGNAL(timeout()), this, SLOT(refresh()));
    
    materials(_scene, _camera, _film);
    
    RenderOptions options;
    options.maxThreadsCount = 0;
    options.antialiasingSampling.count = 4;
    options.antialiasingSampling.jittered = true;
    _renderer = new Renderer(options);
    
    _film->show();
    
    // Launch timer
    _timer.start();
    
    // Launch rendering thread
    _thread = new std::thread(&Main::renderThread, this);
}

Main::~Main() {
    if (_thread) {
        _thread->join();
        delete _thread;
    }
}

void Main::renderThread() {
    startClock("Begin rendering...");
    _renderer->render(*_scene, _camera);
    endClock("Rendering done in");
    //qApp->exit();
}

void Main::refresh() {
    _film->repaint();
    float elapsed = ((float)clock.elapsed()/1000);
    if (clockActive) {
        QString msg = "Ellapsed time: " + QString::number(elapsed) + "s";
        _film->statusBar()->showMessage(msg);
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