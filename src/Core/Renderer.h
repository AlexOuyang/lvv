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
#include "VolumeIntegrator.h"

class Renderer {
public:
    
    static std::shared_ptr<Renderer> Load(const rapidjson::Value& value);

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
    
    Renderer();
    ~Renderer();
    
    void setMaxThreadsCount(int count);
    void setAntialiasingSampling(const SamplingConfig& config);
    void setSurfaceIntegrator(const std::shared_ptr<SurfaceIntegrator>& integrator);
    void setVolumeIntegrator(const std::shared_ptr<VolumeIntegrator>& integrator);
    
    uint_t getIdealThreadCount() const;
    
    void            reset();
    void            preprocess(const Scene& scene, Camera* camera);
    void            render(const Scene& scene, Camera* camera);
    CameraSample*   getSamples(Renderer::Task* task, int* samplesCount) const;
    
    void renderSample(const Scene& scene, Camera* camera, const CameraSample& sample) const;
    
    Spectrum li(const Scene& scene, const Ray& ray) const;
    Spectrum transmittance(const Scene& scene, const Ray& ray) const;

private:
    int                                 _maxThreadsCount;
    SamplingConfig                      _antialiasingSampling;
    std::shared_ptr<SurfaceIntegrator>  _surfaceIntegrator;
    std::shared_ptr<VolumeIntegrator>   _volumeIntegrator;
    int                                 _samplesCount;
};

#endif /* defined(__CSE168_Rendering__Renderer__) */
