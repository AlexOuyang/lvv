//
//  Main.h
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/5/14.
//
//

#ifndef __CSE168_Rendering__Main__
#define __CSE168_Rendering__Main__

#include <thread>
#include <QTimer>
#include <QTime>

#include "Core/Core.h"
#include "Utilities/AssimpImporter.h"
#include "Core/Scene.h"
#include "Core/Renderer.h"
#include "Core/Camera.h"
#include "Films/QtFilm.h"

class Main : public QObject {
    Q_OBJECT
public:
    
    Main();
    ~Main();
    
    void renderThread();
    
    static void startClock(const std::string& message);
    static void endClock(const std::string& message);
    static QTime clock;
    
public slots:
    void refresh();

private:
    Scene*      _scene;
    Renderer*   _renderer;
    Camera*     _camera;
    QtFilm*     _film;
    
    QTimer          _timer;
    std::thread*    _thread;
};

#endif /* defined(__CSE168_Rendering__Main__) */
