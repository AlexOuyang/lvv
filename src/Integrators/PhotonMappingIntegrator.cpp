//
//  PhotonMappingIntegrator.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 5/31/14.
//
//

#include <thread>

#include "PhotonMappingIntegrator.h"

#include "Core/Material.h"
#include "Core/Scene.h"
#include "Core/Renderer.h"

std::shared_ptr<PhotonMappingIntegrator> PhotonMappingIntegrator::Load(const rapidjson::Value& value) {
    std::shared_ptr<PhotonMappingIntegrator> integrator = std::make_shared<PhotonMappingIntegrator>();
    
    if (value.HasMember("globalPhotonsCount")) {
        const rapidjson::Value& count = value["globalPhotonsCount"];
        if (count.IsInt()) {
            integrator->setGlobalPhotonsCount(count.GetInt());
        } else {
            integrator->setGlobalPhotonsCount(count.GetDouble() * 1e6);
        }
    }
    
    if (value.HasMember("causticsPhotonsCount")) {
        const rapidjson::Value& count = value["causticsPhotonsCount"];
        if (count.IsInt()) {
            integrator->setCausticsPhotonsCount(count.GetInt());
        } else {
            integrator->setCausticsPhotonsCount(count.GetDouble() * 1e6);
        }
    }
    
    if (value.HasMember("searchRadius")) {
        integrator->setSearchRadius(value["searchRadius"].GetDouble());
    }
    
    if (value.HasMember("searchCount")) {
        integrator->setSearchRadius(value["searchCount"].GetInt());
    }
    
    return integrator;
}

PhotonMappingIntegrator::PhotonMapNode::PhotonMapNode() : photon(), left(nullptr), right(nullptr) {
}

PhotonMappingIntegrator::PhotonMapNode::~PhotonMapNode() {
    if (left) {
        delete left;
    }
    if (right) {
        delete right;
    }
}

PhotonMappingIntegrator::PhotonMappingIntegrator() :
_globalMap(nullptr), _causticsMap(nullptr),
_globalPhotonsCount(1e6), _causticsPhotonsCount(1e6),
_searchRadius(sqrt(0.0001f)), _searchCount(500) {
}

void PhotonMappingIntegrator::setGlobalPhotonsCount(uint_t count) {
    _globalPhotonsCount = count;
}

void PhotonMappingIntegrator::setCausticsPhotonsCount(uint_t count) {
    _causticsPhotonsCount = count;
}

void PhotonMappingIntegrator::setSearchRadius(float radius) {
    _searchRadius = radius;
}

void PhotonMappingIntegrator::setSearchCount(uint_t count) {
    _searchCount = count;
}

PhotonMappingIntegrator::~PhotonMappingIntegrator() {
    if (_globalMap) {
        delete _globalMap;
    }
    if (_causticsMap) {
        delete _causticsMap;
    }
}

void PhotonMappingIntegrator::preprocess(const Scene& scene, const Camera* camera, const Renderer& renderer) {
    // Cleanup any previous state
    if (_globalMap) {
        delete _globalMap;
    }
    if (_causticsMap) {
        delete _causticsMap;
    }
    
    // Generate maps
    _globalMap = _generatePhotonMap(scene, camera, renderer, _globalPhotonsCount, false);
    _causticsMap = _generatePhotonMap(scene, camera, renderer, _causticsPhotonsCount, true);
}

PhotonMappingIntegrator::PhotonMapNode*
PhotonMappingIntegrator::_generatePhotonMap(const Scene& scene, const Camera*, const Renderer& renderer,
                                            uint_t photonsCount, bool isCausticMap) {
    std::vector<Photon> photons;
    photons.reserve(photonsCount);
    
    // Sample lights
    int nbLights = scene.getLights().size();
    for (const Light* light : scene.getLights()) {
        std::vector<Photon> lightPhotons;
        _traceLightPhotons(scene, renderer, light, &lightPhotons, photonsCount/nbLights, isCausticMap);
        photons.insert(photons.end(), lightPhotons.begin(), lightPhotons.end());
    }
    
    if (photons.size() == 0) {
        return nullptr;
    }
    
    // Build photon map
    return _buildPhotonMapNode(photons, 0, photons.size());
}

void PhotonMappingIntegrator::_traceLightPhotons(const Scene& scene, const Renderer& renderer,
                                                 const Light* light, std::vector<Photon>* photons,
                                                 int photonsCount, bool isCausticMap) const {
    uint_t nbPhotonsTraced = 0;
    photons->reserve(photonsCount);
    
    // Create task threads
    const uint_t threadsCount = renderer.getIdealThreadCount();
    std::thread* threads = new std::thread[threadsCount];
    TraceLightPhotonsTask* tasks = new TraceLightPhotonsTask[threadsCount];
    
    for (uint_t i = 0; i < threadsCount; ++i) {
        tasks[i].photonsCount = photonsCount / threadsCount;
        tasks[i].nbPhotonsTraced = 0;
        tasks[i].scene = &scene;
        tasks[i].light = light;
        tasks[i].isCausticMap = isCausticMap;
        tasks[i].integrator = this;
        threads[i] = std::thread(&TraceLightPhotonsTask::run, &tasks[i]);
    }
    
    for (uint_t i = 0; i < threadsCount; ++i) {
        threads[i].join();
        // Add traced photons to light photons
        photons->insert(photons->end(), tasks[i].photons.begin(), tasks[i].photons.end());
        nbPhotonsTraced += tasks[i].nbPhotonsTraced;
    }
    
    delete[] threads;
    delete[] tasks;
    
    // Divide photons power by number of photons traced
    for (Photon& photon : *photons) {
        photon.power *= 1.f / (float)nbPhotonsTraced;
    }
}

void PhotonMappingIntegrator::TraceLightPhotonsTask::run() {
    while (photons.size() < photonsCount) {
        Ray photonRay;
        Spectrum power = light->samplePhoton(&photonRay.origin, &photonRay.direction);
        ++nbPhotonsTraced;
        integrator->_tracePhoton(*scene, photonRay, power, &photons, photonsCount, isCausticMap);
        
        // If no photons are stored after a lot has been thrown, break to prevent infinite loop
        if (photons.size() == 0 && nbPhotonsTraced > photonsCount) {
            break;
        }
    }
}

void PhotonMappingIntegrator::_tracePhoton(const Scene& scene, const Ray &ray, const Spectrum &power,
                                           std::vector<Photon>* photons, uint_t photonsCount,
                                           bool isCausticMap) const {
    Spectrum photonPower = power;
    Ray photonRay = ray;
    photonRay.depth = 0;
    Intersection isec;
    
    while (scene.intersect(photonRay, &isec)) {
        // Update intersection normal
        isec.applyNormalMapping();
        
        // Sample bsdf
        vec3 wi;
        Material::BxDFType type;
        Spectrum f = isec.material->sampleBSDF(-photonRay.direction, &wi, isec, Material::BSDFAll,
                                               &type);
        
        // If we are generating caustics map, break if we hit a diffuse surface
        if (isCausticMap) {
            if (type & Material::BSDFDiffuse) {
                if (photonRay.depth > 0) {
                    Photon p;
                    p.position = isec.point;
                    p.direction = normalize(-photonRay.direction);
                    p.power = photonPower;
                    photons->push_back(p);
                }
                break;
            }
        } else {
            // Add a photon if we hit a diffuse surface
            if (type & Material::BSDFDiffuse) {
                Photon p;
                p.position = isec.point;
                p.direction = normalize(-photonRay.direction);
                p.power = photonPower;
                photons->push_back(p);
            }
        }

        // Stop tracing if we filled photon map
        if (photons->size() >= photonsCount) {
            break;
        }
        
        // Bounce
        if (!f.isBlack() && photonRay.depth < 5) {
            photonRay.origin = isec.point;
            photonRay.direction = wi;
            photonRay.tmin = isec.rayEpsilon;
            photonRay.tmax = INFINITY;
            photonRay.depth += 1;
            photonPower *= f;
            isec = Intersection();
        } else {
            break;
        }
    }
}

PhotonMappingIntegrator::PhotonMapNode*
PhotonMappingIntegrator::_buildPhotonMapNode(std::vector<Photon> &photons, uint_t start, uint_t end) const {
    PhotonMapNode* node = new PhotonMapNode();
    
    // Compute photons bounding box
    AABB bbox;
    for (uint_t i = start; i < end; ++i) {
        bbox = AABB::Union(bbox, photons[i].position);
    }
    int splitDim = bbox.getMaxDimension();
    // Split photons along the median
    int mid = (start + end)/2;
    std::nth_element(&photons[start], &photons[mid], &photons[end], [&] (const Photon& a, const Photon& b) {
        return a.position[splitDim] < b.position[splitDim];
    });
    // Store photon in current node
    node->photon = photons[mid];
    node->splitDim = splitDim;
    // Recurse on sub-parts if necessary
    if (mid - start > 0) {
        node->left = _buildPhotonMapNode(photons, start, mid);
    }
    if (end - (mid+1) > 0) {
        node->right = _buildPhotonMapNode(photons, mid+1, end);
    }
    return node;
}

Spectrum PhotonMappingIntegrator::li(const Scene& scene, const Renderer& renderer, const Ray& ray,
                                     const Intersection& intersection) const {
    Spectrum l(0.f);
    
    //return _getPhotonMapRadiance(intersection, ray, _globalMap);

    // If primitive is an area light, simply return its emited light
    AreaLight* areaLight = intersection.primitive->getAreaLight();
    if (areaLight) {
        if (ray.type & Ray::DiffuseReflected) {
            return l;
        }
        return areaLight->le(ray, &intersection);
    }
    
    // Sample bsdf to know which type of light we must compute
    vec3 wi;
    Material::BxDFType type;
    Spectrum f = intersection.material->sampleBSDF(-ray.direction, &wi, intersection,
                                                   Material::BSDFAll, &type);
    
    if (f.isBlack()) {
        return l;
    }
    
    // Generate reflected ray
    Ray reflectedRay(ray);
    reflectedRay.origin = intersection.point;
    reflectedRay.direction = wi;
    reflectedRay.tmin = intersection.rayEpsilon;
    reflectedRay.tmax = INFINITY;
    reflectedRay.depth = ray.depth + 1;
    reflectedRay.type = (Ray::Type)(ray.type | ((type & Material::BSDFDiffuse) ?
                                                Ray::DiffuseReflected : Ray::SpecularReflected));
    
    // Diffuse light from diffuse reflection: read in global photon map
    if ((ray.type & Ray::DiffuseReflected) && type == Material::BSDFDiffuse) {
        return _getPhotonMapRadiance(intersection, ray, _globalMap);
    }
    
    // Compute direct illumination
    l += GetDirectLighting(scene, renderer, ray, intersection);
    
    // Get caustic light in photon map
    l += _getPhotonMapRadiance(intersection, ray, _causticsMap);
    
    if (ray.depth < _maxRayDepth) {
        l += f * renderer.li(scene, reflectedRay);
    }
    return l;
}

Spectrum PhotonMappingIntegrator::_getPhotonMapRadiance(const Intersection& intersection,
                                                        const Ray& ray,
                                                        const PhotonMapNode* photonMap) const {
    Spectrum l(0.f);
    
    // Find nearest photons at intersection
    PhotonMapSearchQueue queue;
    float maxDist = _searchRadius*_searchRadius;
    _findNearestPhotons(intersection.point, queue, photonMap, maxDist);

    if (queue.size() == 0) {
        return Spectrum(0.f);
    }

    while (queue.size() > 0) {
        const Photon& photon = queue.top().second->photon;
        if (dot(photon.direction, intersection.normal) > 0) {
            Spectrum fr = intersection.material->evaluateBSDF(-ray.direction, photon.direction, intersection);
            l += fr * photon.power;
        }
        queue.pop();
    }

    l *= 1.f / ((float)M_PI*maxDist);
    
    return l;
}

bool PhotonMappingIntegrator::PhotonMapNodeComparator::operator()(const PhotonMapSearchNode &n1,
                                                                  const PhotonMapSearchNode &n2) {
    return n1.first < n2.first;
}

void PhotonMappingIntegrator::_findNearestPhotons(const vec3& p, PhotonMapSearchQueue& queue,
                                                  const PhotonMapNode* node, float& maxDist) const {
    if (node == nullptr) {
        return;
    }
    float planeDist = p[node->splitDim] - node->photon.position[node->splitDim];
    if (planeDist < 0) {
        _findNearestPhotons(p, queue, node->left, maxDist);
        if (planeDist*planeDist < maxDist) {
            _findNearestPhotons(p, queue, node->right, maxDist);
        }
    } else {
        _findNearestPhotons(p, queue, node->right, maxDist);
        if (planeDist*planeDist < maxDist) {
            _findNearestPhotons(p, queue, node->left, maxDist);
        }
    }
    float d2 = dot(node->photon.position - p, node->photon.position - p);
    if (d2 < maxDist) {
        queue.push(std::make_pair(d2, node));
        if (queue.size() > _searchCount) {
            queue.pop();
        }
        if (queue.size() == _searchCount) {
            maxDist = queue.top().first;
        }
    }
}