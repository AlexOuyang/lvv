//
//  ImageLoading.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 5/8/14.
//
//

#include "ImageLoading.h"

#include <QImage>

std::shared_ptr<Texture> ImageLoading::LoadImage(const std::string& filename) {
    QImage image(filename.c_str());
    
    if (image.isNull()) {
        return nullptr;
    }

    QImage converted = image.convertToFormat(QImage::Format_ARGB32);
    
    // Copy image data to int array
    int size = converted.byteCount();
    uint32_t* data = new uint32_t[size];
    const uint32_t* bits = (const uint32_t*)converted.bits();
    std::copy(bits, bits+(size/sizeof(uint32_t)), data);
    
    std::shared_ptr<IntTexture> texture = std::make_shared<IntTexture>();
    texture->setData(converted.width(), converted.height(), data);
    
    return texture;
}

std::shared_ptr<Texture> ImageLoading::LoadFloatImage(const std::string& filename) {
    QImage image(filename.c_str());
    
    if (image.isNull()) {
        return nullptr;
    }
    
    QImage converted = image.convertToFormat(QImage::Format_ARGB32);
    
    // Copy image data to int array
    int size = converted.byteCount() / (sizeof(uint32_t));
    float* data = new float[size];
    const uint32_t* bits = (const uint32_t*)converted.bits();
    
    for (int i = 0; i < size; ++i) {
        int pixel = bits[i];
        const uint8_t* components = (const uint8_t*)&pixel;
        vec3 value = vec3((float)components[2]/255.f,
                          (float)components[1]/255.f,
                          (float)components[0]/255.f
                          );
        data[i] = (value.x + value.y + value.z) / 3.f;
    }
    
    std::shared_ptr<FloatTexture> texture = std::make_shared<FloatTexture>();
    texture->setData(converted.width(), converted.height(), data);
    
    return texture;
}