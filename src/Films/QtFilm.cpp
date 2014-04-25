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
#include <QDebug>

QtFilm::QtFilm(const vec2& res)
: QMainWindow(), Film(res), _ui(), _image(res.x, res.y, QImage::Format_RGB32) {
    _ui.setupUi(this);
    
    resize(resolution.x + 10, resolution.y + 30);
    
    connect(_ui.actionSave, SIGNAL(triggered()), this, SLOT(saveAction()));
    
    // Fill image of black
    _image.fill(Spectrum().getIntColor());
}

QtFilm::~QtFilm() {
    
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

void QtFilm::addSample(const CameraSample &sample, const Spectrum &L) {
    //vec3 filtered = vec3(1.0f) - glm::exp(L.getColor() * -1.0f);
    vec3 filtered = L.getColor();
    _image.setPixel(sample.pixel.x, sample.pixel.y, Spectrum(filtered).getIntColor());
}

void QtFilm::saveAction() {
    QString filename = QFileDialog::getSaveFileName();
    if (!filename.isEmpty()) {
        _image.save(filename);
    }
}