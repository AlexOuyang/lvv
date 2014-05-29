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

#include <QTimer>
#include <QImage>

class QtFilm : public QMainWindow, public Film {
    Q_OBJECT
public:
    
    static std::shared_ptr<QtFilm> Load(const rapidjson::Value& value,
                                        const vec2& resolution);
    
    QtFilm(const vec2& res);
    virtual ~QtFilm();
    
    void setToogleRenderingCallback(const std::function<void ()> callback);
    
    virtual void paintEvent(QPaintEvent* event);
    
    const QImage& getImage() const;
    
    virtual void addSample(const CameraSample &sample, const Spectrum &L,
                           float weight=1.0f);
    
    void applyFilters();
    
    virtual void keyPressEvent(QKeyEvent * event);
    
public slots:
    void refreshTick();    
    void saveAction();
    
signals:
    void toggleRendering();
    
private:
    Ui_FilmWindow           _ui;
    QImage                  _image;
    QTimer                  _refreshTimer;
    std::vector<vec3>       _buffer;
    std::function<void ()>  _toggleRenderingCallback;
};

#endif /* defined(__CSE168_Rendering__QtFilm__) */
