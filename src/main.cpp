//
//  main.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/3/14.
//
//

#include <fbxsdk.h>

#include <thread>
#include <QApplication>
#include <QThread>
#include <QTime>

#include "Core/ConfigFileReader.h"
#include "Films/ImageFilm.h"
#include "Films/QtFilm.h"

#include <sstream>
#include <iomanip>
#include "Cameras/PerspectiveCamera.h"

int main(int argc, char* argv[]) {
    if (argc <= 1) {
        std::cerr << "Usage: " << argv[0] << " filename" << std::endl;
        return EXIT_FAILURE;
    }
    
    // Init QApplication
    QApplication app(argc, argv);
    
    std::string configFile = argv[1];
    // Get base directory based on config file path
    size_t lastSlash = configFile.find_last_of('/');
    if (lastSlash != std::string::npos) {
        std::string base = configFile.substr(0, lastSlash+1);
        Core::setBaseDirectory(base);
    } else {
        Core::setBaseDirectory("./");
    }
    
    QTime clock;
    clock.start();
    std::cout << "Importing scene..." << std::endl;
    
    ConfigFileReader reader;

    if (!reader.readFile(configFile)) {
        return EXIT_FAILURE;
    }
    
    float elapsed = ((float)clock.elapsed()/1000.f);
    std::cout << "Scene imported in " << elapsed << "s" << std::endl;
    
    std::shared_ptr<Film> film = reader.getFilm();
    std::shared_ptr<Renderer> renderer = reader.getRenderer();
    std::shared_ptr<Scene> scene = reader.getScene();
    
    if (!film) {
        std::cerr << "Error: error while loading film" << std::endl;
        return EXIT_FAILURE;
    }
    if (!renderer) {
        std::cerr << "Error: error while loading renderer" << std::endl;
        return EXIT_FAILURE;
    }
    if (!scene) {
        std::cerr << "Error: error while loading scene" << std::endl;
        return EXIT_FAILURE;
    }
    
    std::shared_ptr<Camera> camera = scene->getCamera();
    
    if (!camera) {
        std::cerr << "Error: error while loading camera" << std::endl;
        return EXIT_FAILURE;
    }
    
    camera->setFilm(film);
    
    // Camera aperture size
    std::shared_ptr<PerspectiveCamera> persp = std::dynamic_pointer_cast<PerspectiveCamera>(camera);
    if (persp) {
         //persp->setApertureSize(0.1f);
    }
    
    // Animation config
    float startFrame = 80.f;
    float endFrame = 190.f;
    float exposureTime = 0.f;
    float deltaFrame = 1.f;
    int nbSamples = 50;
    
    std::shared_ptr<ImageFilm> image = std::dynamic_pointer_cast<ImageFilm>(film);
    
    // ANIMATE
    if (image)
    {
        qDebug() << "Begin rendring animation...";
        // Init animation values
        int frameNum = startFrame;
        float currentFrame = startFrame;
        while (currentFrame <= endFrame) {
            QTime clock;
            clock.start();
            
            scene->evaluateAnimation(currentFrame-(exposureTime/2), currentFrame+(exposureTime/2));
            currentFrame += deltaFrame;
            renderer->preprocess(*scene, camera.get());
            for (int j = 0; j < nbSamples; ++j) {
                renderer->render(*scene, camera.get());
            }
            
            // Create filename
            std::stringstream ss;
            ss << "/Users/gael/Desktop/output_anim/frame_" << std::setfill('0') << std::setw(3) << frameNum << ".bmp";
            image->writeToFile(ss.str());
            image->clear();
            renderer->reset();
            
            float elapsed = ((float)clock.elapsed()/1000.f);
            qDebug() << "Rendered frame" << frameNum << "/" << endFrame << "in" << elapsed << "s";
            
            ++frameNum;
        }
        qDebug() << "Rendering done";
        return EXIT_SUCCESS;
    }
    
    // If we have an image film, render an store
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
        
        // Evaluate scene animation
//        startFrame = 190;
//        scene->evaluateAnimation(startFrame-(exposureTime/2), startFrame+(exposureTime/2));
        
        QTime clock;
        clock.start();
        std::cout << "Rendering photon maps..." << std::endl;
        renderer->preprocess(*scene, camera.get());
        float elapsed = ((float)clock.elapsed()/1000.f);
        std::cout << "Photon maps rendered in " << elapsed << "s" << std::endl;
        std::cout << "Begin rendering..." << std::endl;

        std::thread renderingThread([&] {
            int sampleCount = 0;
            float avgSampleTime = 0.f;
            while (true) {
                if (continueRendering) {
                    ++sampleCount;
                    clock.start();
                    renderer->render(*scene, camera.get());
                    elapsed = ((float)clock.elapsed()/1000.f);
                    avgSampleTime = (sampleCount > 1)
                    ? ((avgSampleTime*(sampleCount-1))+elapsed)/sampleCount : elapsed;
                    qDebug() << "Rendered sample" << sampleCount
                    << "in" << elapsed << "s, avg"
                    << avgSampleTime << "s";
//                    if (sampleCount > 10)
//                        continueRendering = false;
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