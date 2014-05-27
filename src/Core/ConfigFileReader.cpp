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

ConfigFileReader::ConfigFileReader() : _renderer(), _scene() {
}

ConfigFileReader::~ConfigFileReader() {
}

bool ConfigFileReader::readFile(const std::string &filename) {
    // Open and read config file
    std::ifstream file (filename);
    
    if (!file.is_open()) {
        return false;
    }
    
    file.seekg(0, std::ios::end);
    uint_t size = file.tellg();
    char *contents = new char[size];
    file.seekg (0, std::ios::beg);
    file.read (contents, size);
    file.close();
    
    // Parse json file
    Document json;
    json.Parse<0>(contents);
    if (json.HasParseError()) {
        qDebug() << json.GetErrorOffset() << ":" << json.GetParseError();
        qDebug() << QString(contents).mid(json.GetErrorOffset(), 30);
        return false;
    }
    
    // Import configuration
    if (json.HasMember("renderer")) {
        _renderer = Renderer::Load(json["renderer"]);
    }
    if (json.HasMember("scene")) {
        _scene = Scene::Load(json["scene"]);
    }

    delete[] contents;
    return true;
}