//
//  ImageFilm.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 5/24/14.
//
//

#include "ImageFilm.h"

#include <QImage>

ImageFilm::ImageFilm(const vec2& res) : Film(res), _buffer(res.x*res.y) {
    
}

ImageFilm::~ImageFilm() {
    
}

void ImageFilm::addSample(const CameraSample &sample, const Spectrum &L, float weight) {
    vec3 prev = _buffer[sample.pixel.y*resolution.x + sample.pixel.x];
    vec3 newValue = mix(prev, L.getColor(), weight);
    _buffer[sample.pixel.y*resolution.x + sample.pixel.x] = newValue;
}

void ImageFilm::saveToFile(const std::string& filename) {
    QImage img(resolution.x, resolution.y, QImage::Format_ARGB32);
    for (int x = 0; x < resolution.x; ++x) {
        for (int y = 0; y < resolution.y; ++y) {
            vec3 pixel = _buffer[y*resolution.x + x];
            img.setPixel(x, y, Spectrum(pixel).getIntColor());
        }
    }
    img.save(filename.c_str());
}