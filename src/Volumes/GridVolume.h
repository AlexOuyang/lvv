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
    
    virtual float density(const vec3& p) const;
    
    float gridDensity(int x, int y, int z) const;
    
private:
    uint_t  _sizeX, _sizeY, _sizeZ;
    float*  _data;
};

#endif /* defined(__CSE168_Rendering__GridVolume__) */
