//
//  QtFilm.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/3/14.
//
//

#include "QtFilm.h"

#include <QPainter>
#include <QFileDialog>
#include <QKeyEvent>

std::shared_ptr<QtFilm> QtFilm::Load(const rapidjson::Value&, const vec2& resolution) {
    std::shared_ptr<QtFilm> film = std::make_shared<QtFilm>(resolution);
    
    return film;
}

QtFilm::QtFilm(const vec2& res) :
QMainWindow(), Film(res), _ui(), _image(res.x, res.y, QImage::Format_RGB32), _refreshTimer(),
_buffer(res.x*res.y, vec3(0.0f)), _toggleRenderingCallback() {
    _ui.setupUi(this);
    
    resize(resolution.x + 10, resolution.y + 30);
    
    connect(_ui.actionSave, SIGNAL(triggered()), this, SLOT(saveAction()));
    
    // Fill image of black
    _image.fill(Spectrum().getIntColor());
    
    // Setup refresh timer
    _refreshTimer.setInterval(1000/24); // Refresh 24 times per second
    connect(&_refreshTimer, SIGNAL(timeout()), this, SLOT(refreshTick()));
    _refreshTimer.start();
}

QtFilm::~QtFilm() {
    
}

void QtFilm::setToogleRenderingCallback(const std::function<void ()> callback) {
    _toggleRenderingCallback = callback;
}

void QtFilm::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    
    vec2 container = vec2(centralWidget()->width(), centralWidget()->height());
    vec2 size = vec2(_image.width(), _image.height());
    
    // Scale down if necessary
    if (container.x / container.y > size.x / size.y) {
        if (size.y > container.y) {
            size.x = container.y * size.x/size.y;
            size.y = container.y;
        }
    } else {
        if (size.x > container.x) {
            size.y = container.x / (size.x/size.y);
            size.x = container.x;
        }
    }
    
    QRectF target, source(0, 0, _image.width(), _image.height());
    target.setWidth(size.x);
    target.setHeight(size.y);
    target.moveCenter(QPointF(container.x/2, container.y/2));
    
    painter.drawImage(target, _image, source);
}

const QImage& QtFilm::getImage() const {
    return _image;
}

void QtFilm::addSample(const CameraSample &sample, const Spectrum &L, float weight) {
    vec3 prev = _buffer[sample.pixel.y*_image.width() + sample.pixel.x];
    vec3 newValue = mix(prev, L.getColor(), weight);
    _buffer[sample.pixel.y*_image.width() + sample.pixel.x] = newValue;
    _image.setPixel(sample.pixel.x, sample.pixel.y, Spectrum(newValue).getIntColor());
}

void QtFilm::clear() {
    // Fill image of black
    _image.fill(Spectrum().getIntColor());
    _buffer.clear();
    _buffer.resize(resolution.x*resolution.y, vec3(0.0f));
}

void QtFilm::refreshTick() {
    repaint();
}

void QtFilm::saveAction() {
    QString filename = QFileDialog::getSaveFileName();
    if (!filename.isEmpty()) {
        if (_image.save(filename)) {
            qDebug() << "Image saved as" << filename;
        } else {
            qDebug() << "Error: cannot save image as" << filename;
        }
    }
}

void QtFilm::keyPressEvent(QKeyEvent* event) {
    if (event->key() == 32) { // Spacebar
        toggleRendering();
        if (_toggleRenderingCallback) {
            _toggleRenderingCallback();
        }
    }
}

void QtFilm::applyFilters() {
    for (int x = 0; x < _image.width(); ++x) {
        for (int y = 0; y < _image.height(); ++y) {
            vec3 pixel = _buffer[y*_image.width() + x];
            
            // Apply tone mapping
            vec3 filtered;
            //filtered = vec3(1.0f) - glm::exp(filtered * -1.3f);
            filtered = pixel;
            _image.setPixel(x, y, Spectrum(filtered).getIntColor());
            continue;
            
            // Compute gaussian blur
            vec3 gaussian;
            int kernelSizes[] = {5, 10, 20, 40};
            int samples = 8;
            for (uint32 i = 0; i < sizeof(kernelSizes)/sizeof(int); ++i) {
                int kernelSize = kernelSizes[i];
                for (int sampleX = 0; sampleX < samples; ++sampleX) {
                    for (int sampleY = 0; sampleY < samples; ++sampleY) {
                        float samplePixelX = ((float)sampleX/samples);
                        float samplePixelY = ((float)sampleY/samples);
                        // Jitter samples
                        samplePixelX += ((float)rand()/RAND_MAX)*(1.f/samples);
                        samplePixelY += ((float)rand()/RAND_MAX)*(1.f/samples);
                        
                        // Apply gaussian distribution
                        float a = 0.4f * sqrt(-2*log(samplePixelX));
                        float b = 2.0f * M_PI * samplePixelY;
                        samplePixelX = 0.5f + a * sin(b);
                        samplePixelY = 0.5f + a * cos(b);
                        
                        samplePixelX = samplePixelX*2.f - 1.f;
                        samplePixelY = samplePixelY*2.f - 1.f;
                        
                        samplePixelX = glm::clamp(samplePixelX*kernelSize + x, 0.f, _image.width()-1.f);
                        samplePixelY = glm::clamp(samplePixelY*kernelSize + y, 0.f, _image.height()-1.f);
                        vec3 samplePixel = _buffer[(int)samplePixelY*_image.width() + (int)samplePixelX];
                        samplePixel = (samplePixel.x+samplePixel.y+samplePixel.z)/3.f > 1.f ? samplePixel : vec3(0.f);
                        samplePixel *= 0.3f;
                        gaussian += samplePixel / (float)(samples*samples);
                    }
                }
            }
            
            filtered = pixel + gaussian;
            
            _image.setPixel(x, y, Spectrum(filtered).getIntColor());
        }
    }
}