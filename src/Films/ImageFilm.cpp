//
//  ImageFilm.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 5/24/14.
//
//

#include "ImageFilm.h"

#include <QImage>

std::shared_ptr<ImageFilm> ImageFilm::Load(const rapidjson::Value& value, const vec2& resolution) {
    // Check if mandatory values are specified
    if (!value.HasMember("filename")) {
        std::cerr << "ImageFilm error: no filename specified" << std::endl;
        return std::shared_ptr<ImageFilm>();
    }
    const std::string filename = value["filename"].GetString();
    
    std::shared_ptr<ImageFilm> film = std::make_shared<ImageFilm>(resolution);
    film->setFilename(filename);

    return film;
}

ImageFilm::ImageFilm(const vec2& res) : Film(res), _buffer(res.x*res.y) {
    
}

ImageFilm::~ImageFilm() {
    
}

void ImageFilm::setFilename(const std::string& filename) {
    _filename = filename;
}

void ImageFilm::addSample(const CameraSample &sample, const Spectrum &L, float weight) {
    vec3 prev = _buffer[sample.pixel.y*resolution.x + sample.pixel.x];
    vec3 newValue = mix(prev, L.getColor(), weight);
    _buffer[sample.pixel.y*resolution.x + sample.pixel.x] = newValue;
}

void ImageFilm::writeToFile() {
    QImage img(resolution.x, resolution.y, QImage::Format_ARGB32);
    for (int x = 0; x < resolution.x; ++x) {
        for (int y = 0; y < resolution.y; ++y) {
            vec3 pixel = _buffer[y*resolution.x + x];
            img.setPixel(x, y, Spectrum(pixel).getIntColor());
        }
    }
    img.save(_filename.c_str());
}