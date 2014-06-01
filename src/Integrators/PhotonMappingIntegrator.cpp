//
//  PhotonMappingIntegrator.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 5/31/14.
//
//

#include "PhotonMappingIntegrator.h"

#include "Core/Material.h"
#include "Core/Scene.h"

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
_searchRadius(0.1f), _searchCount(200) {
}

PhotonMappingIntegrator::~PhotonMappingIntegrator() {
    if (_globalMap) {
        delete _globalMap;
    }
    if (_causticsMap) {
        delete _causticsMap;
    }
}

void PhotonMappingIntegrator::preprocess(const Scene& scene, const Camera* camera) {
    _globalMap = _generatePhotonMap(scene, camera, _globalPhotonsCount);
    _causticsMap = _generatePhotonMap(scene, camera, _causticsPhotonsCount, true);
}

PhotonMappingIntegrator::PhotonMapNode*
PhotonMappingIntegrator::_generatePhotonMap(const Scene& scene, const Camera*, uint_t photonsCount,
                                            bool causticsMap) {
    std::vector<Photon> photons;
    photons.reserve(photonsCount);
    
    // Sample lights
    int nbLights = scene.getLights().size();
    for (Light* light : scene.getLights()) {
        int nbPhotonsTraced = 0;
        std::vector<Photon> lightPhotons;
        lightPhotons.reserve(photonsCount/nbLights);
        
        while (lightPhotons.size() < (photonsCount/nbLights)) {
            Ray photonRay;
            Spectrum power = light->samplePhoton(&photonRay.origin, &photonRay.direction);
            ++nbPhotonsTraced;
            _tracePhoton(scene, photonRay, power, lightPhotons, photonsCount/nbLights, causticsMap);
        }
        // Divide photons power by number of photons traced
        for (Photon& photon : lightPhotons) {
            photon.power *= 1.f / (float)nbPhotonsTraced;
        }
        qDebug() << nbPhotonsTraced;
        photons.insert(photons.end(), lightPhotons.begin(), lightPhotons.end());
    }
    
    qDebug() << photons.size();
    
    // Build photon map
    return _buildPhotonMapNode(photons, 0, photons.size());
}

void PhotonMappingIntegrator::_tracePhoton(const Scene& scene, const Ray &ray, const Spectrum &power,
                                           std::vector<Photon>& photons, uint_t photonsCount,
                                           bool causticsMap) const {
    Spectrum photonPower = power;
    Ray photonRay = ray;
    photonRay.depth = 0;
    Intersection isec;
    
    while (scene.intersect(photonRay, &isec)) {
        if (causticsMap && photonRay.depth == 0
            && !(isec.material->getBSDFType() & Material::BSDFReflection
                 || isec.material->getBSDFType() & Material::BSDFTransmission)) {
            break;
        }
        
        if (isec.material->getBSDFType() & Material::BSDFDiffuse) {
            Photon p;
            p.position = isec.point;
            p.direction = -photonRay.direction;
            p.power = photonPower;
            photons.push_back(p);
            if (causticsMap) {
                break;
            }
        }
        
        if (photons.size() >= photonsCount) {
            break;
        }
        
        // Sample bsdf
        vec3 wi;
        Spectrum f = isec.material->sampleBSDF(-photonRay.direction, &wi, isec, Material::BSDFAll);
        if (!f.isBlack()) {
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
        if (photonRay.depth > 5) {
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
    const vec3& point = intersection.point;
    
    // Find nearest photons at intersection
    PhotonMapSearchQueue queue;
    float maxDist = _searchRadius*_searchRadius;
    _findNearestPhotons(point, queue, _causticsMap ? _causticsMap : _globalMap, maxDist);
    
    if (queue.size() == 0) {
        return Spectrum(0.f);
    }
    
    Spectrum totalPower(0.f);
    float radius = queue.top().first;
    while (queue.size() > 0) {
        const Photon& photon = queue.top().second->photon;
        totalPower += photon.power;
        queue.pop();
    }
    
    totalPower *= 1.f / ((float)M_PI*radius);
    
    return totalPower;
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
    if (queue.size() == _searchCount) {
        maxDist = queue.top().first;
    }
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
        if (queue.size() == _searchCount) {
            queue.pop();
        }
        queue.push(std::make_pair(d2, node));
    }
}