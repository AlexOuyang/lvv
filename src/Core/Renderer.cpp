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

std::shared_ptr<Renderer> Renderer::Load(const rapidjson::Value& value) {
    std::shared_ptr<Renderer> renderer;
    
    RenderOptions options = RenderOptions::Load(value["options"]);
    renderer = std::make_shared<Renderer>();
    return renderer;
}

Renderer::Renderer(RenderOptions options)
: options(options), _surfaceIntegrator(nullptr), _volumeIntegrator(nullptr), _samplesCount(0) {
    _surfaceIntegrator = options.createSurfaceIntegrator();
    _volumeIntegrator = options.createVolumeIntegrator();
}

Renderer::~Renderer() {
    if (_surfaceIntegrator) {
        delete _surfaceIntegrator;
    }
    if (_volumeIntegrator) {
        delete _volumeIntegrator;
    }
}

void Renderer::render(const Scene& scene, Camera* camera) {
    _samplesCount += 1;
    
    // Determine number of tasks
     // Compute ideal task count
    int pixelsCount = camera->getFilm()->resolution.x * camera->getFilm()->resolution.y;
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
    
    if (options.maxThreadsCount > 0) {
        threadPool.setMaxThreadCount(options.maxThreadsCount);
    }
    
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
    const vec2& resolution = task->camera->getFilm()->resolution;
    
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
            samples[y*subSampleWidth + x].pixel.x = subSamplesStart.x + x;
            samples[y*subSampleWidth + x].pixel.y = subSamplesStart.y + y;
            samples[y*subSampleWidth + x].position.x = (subSamplesStart.x + x) / resolution.x;
            samples[y*subSampleWidth + x].position.y = 1 - ((subSamplesStart.y + y + 1)
                                                            / resolution.y);
            samples[y*subSampleWidth + x].pixelSize.x = 1.0f / resolution.x;
            samples[y*subSampleWidth + x].pixelSize.y = 1.0f / resolution.y;
        }
    }
    
    return samples;
}

void Renderer::renderSample(const Scene& scene, Camera* camera, const CameraSample& sample) const {
    Spectrum ls;
    
    // Create sub-samples for anti-aliasing
    int samplesCount = options.antialiasingSampling.count;
    for (int subSampleX = 0; subSampleX < samplesCount; ++subSampleX) {
        for (int subSampleY = 0; subSampleY < samplesCount; ++subSampleY) {
            // Create sub sample based on sampling method
            CameraSample subSample = sample;
            
            vec2 subSampleDelta = vec2((float)subSampleX/samplesCount,
                                       (float)subSampleY/samplesCount);
            
            vec2 subSampleSize = vec2(1.0f) / (float)samplesCount;
            
            if (options.antialiasingSampling.jittered) {
                subSampleDelta += (vec2((float)rand()/RAND_MAX, (float)rand()/RAND_MAX)
                                   * subSampleSize);
            } else {
                subSampleDelta += (vec2(0.5f, 0.5f) * subSampleSize);
            }

            switch (options.antialiasingSampling.distribution) {
                case SamplingConfig::GaussDistribution: {
                    float a = 0.4f * sqrt(-2*log(subSampleDelta.x));
                    float b = 2.0f * M_PI * subSampleDelta.y;
                    subSampleDelta.x = 0.5f + a * sin(b);
                    subSampleDelta.y = 0.5f + a * cos(b);
                    break;
                }
                case SamplingConfig::ShirleyDistribution: {
                    if (subSampleDelta.x < 0.5f) {
                        subSampleDelta.x = -0.5f + sqrt(2*subSampleDelta.x);
                    } else {
                        subSampleDelta.x = 1.5f - sqrt(2-2*subSampleDelta.x);
                    }
                    
                    if (subSampleDelta.y < 0.5f) {
                        subSampleDelta.y = -0.5f + sqrt(2*subSampleDelta.y);
                    } else {
                        subSampleDelta.y = 1.5f - sqrt(2-2*subSampleDelta.y);
                    }
                    break;
                }
                default: {
                    break;
                }
            }
            
            subSample.position += subSampleDelta * sample.pixelSize;
            
            // Generate primary ray
            Ray ray;
            float rayWeight = camera->generateRay(subSample, &ray);
            
            // Scale ray weight by sample contribution
            rayWeight *= 1.0f / ((float)(samplesCount*samplesCount));
            
            // Compute amount of light arriving along the ray
            ls += rayWeight * li(scene, ray);
        }
    }
    
    // Add sample contribution to camera film
    camera->getFilm()->addSample(sample, ls, 1.0f/(float)_samplesCount);
}

Spectrum Renderer::li(const Scene &scene, const Ray &ray) const {
    Intersection intersection;
    Spectrum li(0);
    
    // Intersect ray with scene geometry
    if (scene.intersect(ray, &intersection)) {
        li = _surfaceIntegrator->li(scene, *this, ray, intersection);
    } else {
        // Handle ray that doesn't intersect any geometry
        for (Light* light : scene.getLights()) {
            li += light->le(ray);
        }
    }
    
    // Compute light coming from participating media
    Spectrum t;
    Spectrum lv = _volumeIntegrator->li(scene, *this, ray, &t);
    
    return t * li + lv;
}

Spectrum Renderer::transmittance(const Scene &scene, const Ray &ray) const {
    return _volumeIntegrator->transmittance(scene, *this, ray);
}