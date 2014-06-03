//
//  PhotonMappingIntegrator.h
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 5/31/14.
//
//

#ifndef __CSE168_Rendering__PhotonMappingIntegrator__
#define __CSE168_Rendering__PhotonMappingIntegrator__

#include "Core/Core.h"
#include "Core/SurfaceIntegrator.h"
#include "Core/Light.h"

#include <queue>
#include <mutex>

class PhotonMappingIntegrator : public SurfaceIntegrator {
public:
    
    static std::shared_ptr<PhotonMappingIntegrator> Load(const rapidjson::Value& value);
    
    PhotonMappingIntegrator();
    virtual ~PhotonMappingIntegrator();
    
    void setGlobalPhotonsCount(uint_t count);
    void setCausticsPhotonsCount(uint_t count);
    void setSearchRadius(float radius);
    void setSearchCount(uint_t count);
    
    virtual void preprocess(const Scene& scene, const Camera* camera,
                            const Renderer& renderer);
    
    virtual Spectrum li(const Scene& scene, const Renderer& renderer, const Ray& ray,
                        const Intersection& Intersection) const;
    
private:
    struct Photon {
        vec3        position;
        vec3        direction;
        Spectrum    power;
    };
    
    struct PhotonMapNode {
        PhotonMapNode();
        ~PhotonMapNode();
        
        Photon          photon;
        uint_t          splitDim;
        PhotonMapNode*  left;
        PhotonMapNode*  right;
    };
    
    typedef std::pair<float, const PhotonMapNode*> PhotonMapSearchNode;
    
    class PhotonMapNodeComparator {
    public:
        bool operator()(const PhotonMapSearchNode& n1, const PhotonMapSearchNode& n2);
    };
    
    typedef std::priority_queue
    <PhotonMapSearchNode, std::vector<PhotonMapSearchNode>,
    PhotonMapNodeComparator> PhotonMapSearchQueue;
    
    class TraceLightPhotonsTask {
    public:
        void run();
        
        std::vector<Photon>             photons;
        uint_t                          photonsCount;
        uint_t                          nbPhotonsTraced;
        const Scene*                    scene;
        const Light*                    light;
        bool                            isCausticMap;
        const PhotonMappingIntegrator*  integrator;
    };
    
    PhotonMapNode* _generatePhotonMap(const Scene& scene, const Camera*,
                                      const Renderer& renderer,
                                      uint_t photonsCount, bool isCausticMap);
    
    void _traceLightPhotons(const Scene& scene, const Renderer& renderer,
                            const Light* light, std::vector<Photon>* photons,
                            int photonsCount, bool isCausticMap) const;
    void _tracePhoton(const Scene& scene, const Ray& ray, const Spectrum& power,
                      std::vector<Photon>* photons, uint_t photonsCount,
                      bool isCausticMap) const;
    
    PhotonMapNode* _buildPhotonMapNode(std::vector<Photon>& photons,
                                       uint_t start, uint_t end) const;
    
    Spectrum    _getPhotonMapRadiance(const Intersection& intersection,
                                      const Ray& ray,
                                      const PhotonMapNode* photonMap) const;
    void        _findNearestPhotons(const vec3& p, PhotonMapSearchQueue& queue,
                                    const PhotonMapNode* node, float& maxDist) const;
    
    PhotonMapNode*      _globalMap;
    PhotonMapNode*      _causticsMap;
    uint_t              _globalPhotonsCount;
    uint_t              _causticsPhotonsCount;
    float               _searchRadius;
    uint_t              _searchCount;
};

#endif /* defined(__CSE168_Rendering__PhotonMappingIntegrator__) */
