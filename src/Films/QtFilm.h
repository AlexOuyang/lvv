//
//  QtFilm.h
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/3/14.
//
//

#ifndef __CSE168_Rendering__QtFilm__
#define __CSE168_Rendering__QtFilm__

#include "Core/Film.h"
#include "ui_FilmWindow.h"

#include <QImage>

class QtFilm : public QMainWindow, public Film {
    Q_OBJECT
public:
    
    QtFilm(const vec2& res);
    virtual ~QtFilm();
    
    virtual void paintEvent(QPaintEvent* event);
    
    const QImage& getImage() const;
    
    virtual void addSample(const CameraSample &sample, const Spectrum &L, float weight=1.0f);
    
protected:
    virtual void keyPressEvent(QKeyEvent * event);
    
public slots:
    void saveAction();
    
signals:
    void toggleRendering();
    
private:
    Ui_FilmWindow       _ui;
    QImage              _image;
    std::vector<vec3>   _buffer;
};

#endif /* defined(__CSE168_Rendering__QtFilm__) */
