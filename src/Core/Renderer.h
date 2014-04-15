    //
//  Renderer.h
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/3/14.
//
//

#ifndef __CSE168_Rendering__Renderer__
#define __CSE168_Rendering__Renderer__

#include <QRunnable>

#include "Core.h"
#include "RenderOptions.h"
#include "Scene.h"
#include "Camera.h"
#include "SurfaceIntegrator.h"

class Renderer {
public:
    
    class Task : public QRunnable {
    public:
        
        static int NumSystemCores();
        
        Task();
        virtual ~Task();
        
        const Renderer* renderer;
        const Scene*    scene;
        Camera*         camera;
        
        int         taskNumber;
        int         tasksCount;
        
    protected:
        virtual void run();
    };
    
    Renderer(RenderOptions options=RenderOptions());
    ~Renderer();
    
    void            render(const Scene& scene, Camera* camera);
    CameraSample*   getSamples(Renderer::Task* task, int* samplesCount) const;
    
    void renderSample(const Scene& scene, Camera* camera, const CameraSample& sample) const;
    
    Spectrum li(const Scene& scene, const Ray& ray) const;
    
private:
    RenderOptions       _options;
    SurfaceIntegrator*  _surfaceIntegrator;
};

#endif /* defined(__CSE168_Rendering__Renderer__) */
