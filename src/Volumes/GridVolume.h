//
//  GridVolume.h
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 5/30/14.
//
//

#ifndef __CSE168_Rendering__GridVolume__
#define __CSE168_Rendering__GridVolume__

#include "Core/Core.h"
#include "Volumes/DensityVolume.h"

class GridVolume : public DensityVolume {
public:
    
    GridVolume();
    virtual ~GridVolume();
    
    bool loadData(const std::string& filename);
    void setFrame(int frame);
    
    virtual float density(const vec3& p) const;
    virtual float dataAtPoint(float* data, const vec3& p) const;
    
    float gridData(float* data, int x, int y, int z) const;
    
    virtual Spectrum le(const vec3& p) const;
    
    struct Frame {
        float*  density;
        float*  temperature;
    };
    
private:
    uint_t              _sizeX, _sizeY, _sizeZ;
    std::vector<Frame>  _frames;
    float*              _density;
    float*              _temperature;
};

#endif /* defined(__CSE168_Rendering__GridVolume__) */
