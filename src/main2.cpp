//
//  main.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/3/14.
//
//

#include <QApplication>

#include "Utilities/Main.h"

#include "Core/ConfigFileReader.h"

int main(int argc, char* argv[]) {
    ConfigFileReader reader;
    
    if (!reader.readFile("/Users/gael/Desktop/Courses/CSE_168/scenes/chess.json")) {
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
    
    renderer->render(*scene, camera.get());
    
    std::shared_ptr<ImageFilm> image = std::dynamic_pointer_cast<ImageFilm>(film);
    if (image) {
        image->writeToFile();
    }
}