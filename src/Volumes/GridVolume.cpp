//
//  GridVolume.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 5/30/14.
//
//

#include "GridVolume.h"

#include "Core/ConfigFileReader.h"

GridVolume::GridVolume() : DensityVolume(), _sizeX(0), _sizeY(0), _sizeZ(0), _data(nullptr) {
    
}

GridVolume::~GridVolume() {
    if (_data) {
        delete[] _data;
    }
}

bool GridVolume::loadData(const std::string& filename) {
    // Import data
    std::string contents;
    if (!ConfigFileReader::LoadFileContents(filename, contents)) {
        return false;
    }
    
    // Parse json file
    rapidjson::Document json;
    json.Parse<0>(contents.c_str());
    if (json.HasParseError()) {
        std::cerr << json.GetErrorOffset() << ": " << json.GetParseError() << std::endl;
        std::cerr << contents.substr(json.GetErrorOffset(), 30) << std::endl;
        return false;
    }
    
    if (!json.HasMember("size")) {
        std::cerr << "GridVolume import error: no size specified" << std::endl;
        return false;
    }
    if (!json.HasMember("data")) {
        std::cerr << "GridVolume import error: no data specified" << std::endl;
        return false;
    }
    
    // Get size and actual data
    const rapidjson::Value& sizeValue = json["size"];
    const rapidjson::Value& dataValue = json["data"];
    
    if (!sizeValue.IsArray() || sizeValue.Size() != 3) {
        std::cerr << "GridVolume import error: invalid size, expected a 3D int vector" << std::endl;
        return false;
    }
    
    _sizeX = sizeValue[0u].GetInt();
    _sizeY = sizeValue[1u].GetInt();
    _sizeZ = sizeValue[2u].GetInt();
    
    if (!dataValue.IsArray() || dataValue.Size() != (_sizeX*_sizeY*_sizeZ)) {
        std::cerr << "GridVolume import error: invalid data, expected a float array of size x*y*z"
        << std::endl;
        return false;
    }
    
    // Load data
    _data = new float[_sizeX*_sizeY*_sizeZ];
    for (uint_t i = 0; i < _sizeX*_sizeY*_sizeZ; ++i) {
        _data[i] = dataValue[i].GetDouble();
    }
    
    return true;
}

float GridVolume::density(const vec3& p) const {
    int nx = 100, ny = 100, nz = 40;
    
    if (!_bounds.intersectP(p)) {
        return 0.f;
    }
    
    // Compute voxel coordinates and offsets for _Pobj_
    vec3 vox = (p - _bounds.min) / (_bounds.max - _bounds.min);
    vox.x = vox.x * nx - .5f;
    vox.y = vox.y * ny - .5f;
    vox.z = vox.z * nz - .5f;
    int vx = (int)floorf(vox.x), vy = (int)floorf(vox.y), vz = (int)floorf(vox.z);
    float dx = vox.x - vx, dy = vox.y - vy, dz = vox.z - vz;
    
    // Trilinearly interpolate density values to compute local density
    float d00 = mix(gridDensity(vx, vy, vz),     gridDensity(vx+1, vy, vz), dx);
    float d10 = mix(gridDensity(vx, vy+1, vz),   gridDensity(vx+1, vy+1, vz), dx);
    float d01 = mix(gridDensity(vx, vy, vz+1),   gridDensity(vx+1, vy, vz+1), dx);
    float d11 = mix(gridDensity(vx, vy+1, vz+1), gridDensity(vx+1, vy+1, vz+1), dx);
    float d0 = mix(d00, d10, dy);
    float d1 = mix(d01, d11, dy);
    return mix(d0, d1, dz);
}

float GridVolume::gridDensity(int x, int y, int z) const {
    if (!_data) {
        return 0.f;
    }
    
    x = clamp(x, 0, (int)_sizeX-1);
    y = clamp(y, 0, (int)_sizeY-1);
    z = clamp(z, 0, (int)_sizeZ-1);
    return _data[z*_sizeX*_sizeY + y*_sizeX + x];
}