//
//  main.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/3/14.
//
//

#include <thread>
#include <QApplication>
#include <QThread>
#include <QTime>

#include "Core/ConfigFileReader.h"
#include "Films/ImageFilm.h"
#include "Films/QtFilm.h"

int main(int argc, char* argv[]) {
    if (argc <= 1) {
        std::cerr << "Usage: " << argv[0] << " filename" << std::endl;
        return EXIT_FAILURE;
    }
    
    // Init QApplication
    QApplication app(argc, argv);
    
    ConfigFileReader reader;

    if (!reader.readFile(argv[1])) {
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
    
    // If we have an image film, render an store
    std::shared_ptr<ImageFilm> image = std::dynamic_pointer_cast<ImageFilm>(film);
    if (image) {
        renderer->render(*scene, camera.get());
        image->writeToFile();
        return EXIT_SUCCESS;
    }
    
    // Else open the window and launch rendering thread
    std::shared_ptr<QtFilm> window = std::dynamic_pointer_cast<QtFilm>(film);
    if (window) {
        bool continueRendering = true;
        
        window->setToogleRenderingCallback([&] {
            continueRendering = !continueRendering;
        });
        
        window->show();
        
        std::cout << "Begin rendering..." << std::endl;

        std::thread renderingThread([&] {
            int sampleCount = 0;
            float avgSampleTime = 0.f;
            while (true) {
                if (continueRendering) {
                    ++sampleCount;
                    QTime clock;
                    clock.start();
                    renderer->render(*scene, camera.get());
                    float elapsed = ((float)clock.elapsed()/1000.f);
                    avgSampleTime = (sampleCount > 1)
                    ? ((avgSampleTime*(sampleCount-1))+elapsed)/sampleCount : elapsed;
                    qDebug() << "Rendered sample" << sampleCount
                    << "in" << elapsed << "s, avg"
                    << avgSampleTime << "s";
                } else {
                    QThread::msleep(100);
                }
            }
        });
        app.exec();
        renderingThread.join();
        return EXIT_SUCCESS;
    }
    
    std::cerr << "Error: Unhandled film type" << std::endl;
    return EXIT_FAILURE;
}