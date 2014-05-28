//
//  Film.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/3/14.
//
//

#include "Film.h"

#include "Films/ImageFilm.h"

std::shared_ptr<Film> Film::Load(const rapidjson::Value& value) {
    // Check if mandatory values are specified
    if (!value.HasMember("type")) {
        std::cerr << "Film error: no type specified" << std::endl;
        return std::shared_ptr<Film>();
    }
    if (!value.HasMember("resolution")) {
        std::cerr << "Film error: no resolution specified" << std::endl;
        return std::shared_ptr<Film>();
    }
    
    // Get resolution
    const rapidjson::Value& resolutionValue = value["resolution"];
    if (!resolutionValue.IsArray() || resolutionValue.Size() != 2) {
        std::cerr << "Film error: invalid resolution specified" << std::endl;
        return std::shared_ptr<Film>();
    }
    vec2 resolution = vec2((float)resolutionValue[0u].GetDouble(),
                           (float)resolutionValue[1u].GetDouble());
    
    // Create film based on given type
    std::string type = value["type"].GetString();
    std::transform(type.begin(), type.end(), type.begin(), ::tolower);
    
    std::shared_ptr<Film> film;
    if (type == "image") {
        film = ImageFilm::Load(value, resolution);
    } else {
        std::cerr << "Film error: unknown film \"" << type << "\"" << std::endl;
        return std::shared_ptr<Film>();
    }
    
    return film;
}

Film::Film(const vec2& res) : resolution(res) {
    
}

Film::~Film() {
    
}