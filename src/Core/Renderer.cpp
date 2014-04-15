//
//  Renderer.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/3/14.
//
//

#include "Renderer.h"

#include "Core/Intersection.h"

#include <QThreadPool>
#include <QDebug>
#include <random>
#include <algorithm>

int Renderer::Task::NumSystemCores() {
    int num = QThread::idealThreadCount();
    return glm::max(1, num);
}

Renderer::Task::Task() {
    setAutoDelete(false);
}

Renderer::Task::~Task() {
}

void Renderer::Task::run() {
    int samplesCount = 0;
    
    // Get samples for current task
    CameraSample* samples = renderer->getSamples(this, &samplesCount);
    if (!samples || !samplesCount) {
        return;
    }
    
    // Render samples
    for (int i = 0; i < samplesCount; ++i) {
        renderer->renderSample(*scene, camera, samples[i]);
    }
    
    // Delete samples
    delete[] samples;
}

Renderer::Renderer(RenderOptions options)
: _options(options), _surfaceIntegrator(nullptr) {
    _surfaceIntegrator = _options.createSurfaceIntegrator();
}

Renderer::~Renderer() {
    if (_surfaceIntegrator) {
        delete _surfaceIntegrator;
    }
}

void Renderer::render(const Scene& scene, Camera* camera) {
    // Determine number of tasks
     // Compute ideal task count
    int pixelsCount = camera->film->resolution.x * camera->film->resolution.y;
    int tasksCount = glm::max(32 * Task::NumSystemCores(), pixelsCount / (16 * 16));
    tasksCount = Core::roundUpPow2(tasksCount);
    
    // Create tasks
    Renderer::Task** tasks = new Renderer::Task*[tasksCount];
    for (int i = 0; i < tasksCount; ++i) {
        tasks[i] = new Renderer::Task();
        tasks[i]->renderer = this;
        tasks[i]->scene = &scene;
        tasks[i]->camera = camera;
        tasks[i]->taskNumber = i;
        tasks[i]->tasksCount = tasksCount;
    }
    
    // Shuffle tasks
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(&tasks[0], &tasks[tasksCount], g);
    
    // Create thread pool
    QThreadPool threadPool;
    //threadPool.setMaxThreadCount(1);
    
    // Launch tasks
    for (int i = 0; i < tasksCount; ++i) {
        threadPool.start(tasks[i]);
    }
    
    // Wait for end of tasks
    threadPool.waitForDone();
    
    // Delete tasks
    for (int i = 0; i < tasksCount; ++i) {
        delete tasks[i];
    }
    delete[] tasks;
}

CameraSample* Renderer::getSamples(Renderer::Task *task, int* samplesCount) const {
    const vec2& resolution = task->camera->film->resolution;
    
    // Compute divisions size
    int countLog = glm::log2((float)task->tasksCount);
    int divisionsXCount = pow(2.0, (float)((int)countLog/2));
    int divisionsYCount = pow(2.0, (float)((int)countLog-(countLog/2)));
    // Get highest division count for highest dimension
    if (resolution.x / resolution.y > 1) {
        std::swap(divisionsXCount, divisionsYCount);
    }
    
    int divisionsWidth = glm::max(1, (int)resolution.x / divisionsXCount);
    int divisionsHeight = glm::max(1, (int)resolution.y / divisionsYCount);
    
    vec2 subSamplesStart, subSamplesEnd;

    subSamplesStart.x = divisionsWidth * (task->taskNumber%divisionsXCount);
    subSamplesStart.y = divisionsHeight * (task->taskNumber/divisionsXCount);
    
    if (((task->taskNumber+1)%divisionsXCount) == 0) {
        subSamplesEnd.x = resolution.x;
    } else {
        subSamplesEnd.x = divisionsWidth * ((task->taskNumber+1)%divisionsXCount);
    }
    if (((task->taskNumber/divisionsXCount)+1) == divisionsYCount) {
        subSamplesEnd.y = resolution.y;
    } else {
        subSamplesEnd.y = divisionsHeight * ((task->taskNumber/divisionsXCount)+1);
    }
    
    if (subSamplesEnd.x >= resolution.x || subSamplesEnd.y >= resolution.y) {
        if (subSamplesStart.x >= resolution.x || subSamplesStart.y >= resolution.y) {
            *samplesCount = 0;
            return nullptr;
        }
        subSamplesEnd.x = glm::min(resolution.x, subSamplesEnd.x);
        subSamplesEnd.y = glm::min(resolution.y, subSamplesEnd.y);
    }
    
    // Compute number of samples for this task (number of pixels for the image section)
    int subSampleWidth = (subSamplesEnd - subSamplesStart).x;
    int subSampleHeight = (subSamplesEnd - subSamplesStart).y;
    *samplesCount = subSampleWidth * subSampleHeight;
    CameraSample* samples = new CameraSample[*samplesCount];

    for (int x = 0; x < subSampleWidth; ++x) {
        for (int y = 0; y < subSampleHeight; ++y) {
            samples[y*subSampleWidth + x].image.x = subSamplesStart.x + x;
            samples[y*subSampleWidth + x].image.y = subSamplesStart.y + y;
            samples[y*subSampleWidth + x].lens.x = (subSamplesStart.x + x + 0.5) / resolution.x;
            samples[y*subSampleWidth + x].lens.y = 1 - ((subSamplesStart.y + y + 0.5)
                                                        / resolution.y);
        }
    }
    
    return samples;
}

void Renderer::renderSample(const Scene& scene, Camera* camera, const CameraSample& sample) const {
    Ray ray;
    
    // Generate primary ray
    float rayWeight = camera->generateRay(sample, &ray);
    
    // Compute amount of light arriving along the ray
    Spectrum ls = rayWeight * li(scene, ray);
    
    // Add sample contribution to camera film
    camera->film->addSample(sample, ls);
}

Spectrum Renderer::li(const Scene &scene, const Ray &ray) const {
    Intersection intersection;
    Spectrum li(0);
    
    if (scene.intersect(ray, &intersection)) {
        li = _surfaceIntegrator->li(scene, *this, ray, intersection);
    } else {
        // Handle ray that doesn't intersect any geometry
        for (Light* light : scene.lights) {
            li += light->le(ray);
        }
    }
    
    return li;
}