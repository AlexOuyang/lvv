//
//  ConfigFileReader.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 5/25/14.
//
//

#include "ConfigFileReader.h"

#include <fstream>

using namespace rapidjson;

ConfigFileReader::ConfigFileReader() : _film(), _renderer(), _scene() {
}

ConfigFileReader::~ConfigFileReader() {
}

bool ConfigFileReader::readFile(const std::string &filename) {
    // Open and read config file
    std::string contents;
    LoadFileContents(filename, contents);
    
    // Parse json file
    Document json;
    json.Parse<0>(contents.c_str());
    if (json.HasParseError()) {
        std::cerr << json.GetErrorOffset() << ": " << json.GetParseError() << std::endl;
        std::cerr << contents.substr(json.GetErrorOffset(), 30) << std::endl;
        return false;
    }
    
    // Import configuration
    if (json.HasMember("film")) {
        _film = Film::Load(json["film"]);
        if (!_film) {
            std::cerr << "ConfigFileReader error: error while loading film" << std::endl;
        }
    }
    if (json.HasMember("renderer")) {
        _renderer = Renderer::Load(json["renderer"]);
        if (!_renderer) {
            std::cerr << "ConfigFileReader error: error while loading renderer" << std::endl;
        }
    }
    if (json.HasMember("scene")) {
        _scene = Scene::Load(json["scene"]);
        if (!_scene) {
            std::cerr << "ConfigFileReader error: error while loading scene" << std::endl;
        }
    }

    return true;
}

std::shared_ptr<Film> ConfigFileReader::getFilm() const {
    return _film;
}

std::shared_ptr<Renderer> ConfigFileReader::getRenderer() const {
    return _renderer;
}

std::shared_ptr<Scene> ConfigFileReader::getScene() const {
    return _scene;
}

bool ConfigFileReader::LoadFileContents(std::string filename, std::string& contents) {
    if (filename[0] != '/') {
        filename = Core::baseDirectory + filename;
    }
    
    std::ifstream file (filename);
    
    if (!file.is_open()) {
        std::cerr << "ConfigFileReader error: cannot open file \"" << contents << std::endl;
        return false;
    }
    
    file.seekg(0, std::ios::end);
    contents.resize(file.tellg());
    file.seekg (0, std::ios::beg);
    file.read(&contents[0], contents.size());
    file.close();
    return true;
}

vec3 ConfigFileReader::LoadVec3(const rapidjson::Value& value) {
    if (!value.IsArray() || value.Size() != 3) {
        std::cerr << "ConfigFileReader error: Invalid vector specified" << std::endl;
    }
    return vec3((float)value[0u].GetDouble(),
                (float)value[1u].GetDouble(),
                (float)value[2u].GetDouble());
}