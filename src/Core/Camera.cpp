//
//  Camera.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 5/27/14.
//
//

#include "Camera.h"

#include "Cameras/PerspectiveCamera.h"

std::shared_ptr<Camera> Camera::Load(const rapidjson::Value& value) {
    // Check if mandatory values are specified
    if (!value.HasMember("type")) {
        std::cerr << "Camera error: no type specified" << std::endl;
        return std::shared_ptr<Camera>();
    }
    
    // Create camera based on given type
    std::string type = value["type"].GetString();
    std::transform(type.begin(), type.end(), type.begin(), ::tolower);
    
    std::shared_ptr<Camera> camera;
    if (type == "perspective") {
        camera = PerspectiveCamera::Load(value);
    } else {
        std::cerr << "Error error: unknown camera \"" << type << "\"" << std::endl;
        return std::shared_ptr<Camera>();
    }
    
    return camera;
}

Camera::Camera() : _name(), _film() {
    
}

Camera::~Camera() {
    
}

void Camera::setName(const std::string& name) {
    _name = name;
}

const std::string& Camera::getName() const {
    return _name;
}

void Camera::setFilm(const std::shared_ptr<Film>& film) {
    _film = film;
}

std::shared_ptr<Film> Camera::getFilm() const {
    return _film;
}