//
//  GridVolume.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 5/30/14.
//
//

#include "GridVolume.h"

#include "Core/ConfigFileReader.h"

GridVolume::GridVolume() :
DensityVolume(), _sizeX(0), _sizeY(0), _sizeZ(0), _frames(), _density(nullptr), _temperature(nullptr) {
    
}

GridVolume::~GridVolume() {
    for (const Frame& frame : _frames) {
        if (frame.density) {
            delete[] frame.density;
        }
        if (frame.temperature) {
            delete[] frame.temperature;
        }
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
    if (!json.HasMember("frames")) {
        std::cerr << "GridVolume import error: no frame specified" << std::endl;
        return false;
    }
    
    // Get size and actual data
    const rapidjson::Value& sizeValue = json["size"];
    
    if (!sizeValue.IsArray() || sizeValue.Size() != 3) {
        std::cerr << "GridVolume import error: invalid size, expected a 3D int vector" << std::endl;
        return false;
    }
    
    _sizeX = sizeValue[0u].GetInt();
    _sizeY = sizeValue[1u].GetInt();
    _sizeZ = sizeValue[2u].GetInt();
    
    const rapidjson::Value& framesValue = json["frames"];
    if (!framesValue.IsArray()) {
        std::cerr << "GridVolume import error: invalid frame array" << std::endl;
        return false;
    }
    
    _frames.reserve(framesValue.Size());
    
    for (int i = 0; i < framesValue.Size(); ++i) {
        const rapidjson::Value& frameValue = framesValue[i];
        
        const rapidjson::Value& densityValue = frameValue["density"];
        
        if (!densityValue.IsArray() || densityValue.Size() != (_sizeX*_sizeY*_sizeZ)) {
            std::cerr << "GridVolume import error: invalid data, expected a float array of size x*y*z"
            << std::endl;
            return false;
        }
        
        // Load data
        float* density = new float[_sizeX*_sizeY*_sizeZ];
        for (uint_t i = 0; i < _sizeX*_sizeY*_sizeZ; ++i) {
            density[i] = densityValue[i].GetDouble();
        }
        
        // Load temperature data
        float* temperature = nullptr;
        if (frameValue.HasMember("temperature")) {
            const rapidjson::Value& tempValue = frameValue["temperature"];
            if (!tempValue.IsArray() || tempValue.Size() != (_sizeX*_sizeY*_sizeZ)) {
                std::cerr << "GridVolume import error: invalid temperature data, expected a float array of size x*y*z"
                << std::endl;
                return false;
            }
            
            // Load data
            temperature = new float[_sizeX*_sizeY*_sizeZ];
            for (uint_t i = 0; i < _sizeX*_sizeY*_sizeZ; ++i) {
                temperature[i] = tempValue[i].GetDouble();
            }
        }
        Frame f;
        f.density = density;
        f.temperature = temperature;
        _frames.push_back(f);
    }
    
    setFrame(0);
    
    return true;
}

void GridVolume::setFrame(int frame) {
    frame -= 106;
    Frame f = _frames[glm::clamp(frame, 0, (int)_frames.size()-1)];
    _density = f.density;
    _temperature = f.temperature;
}

float GridVolume::density(const vec3& p) const {
    return dataAtPoint(_temperature, p);
}

float GridVolume::dataAtPoint(float *data, const vec3 &p) const {
    int nx = _sizeX, ny = _sizeY, nz = _sizeZ;
    
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
    float d00 = mix(gridData(data, vx, vy, vz),     gridData(data, vx+1, vy, vz), dx);
    float d10 = mix(gridData(data, vx, vy+1, vz),   gridData(data, vx+1, vy+1, vz), dx);
    float d01 = mix(gridData(data, vx, vy, vz+1),   gridData(data, vx+1, vy, vz+1), dx);
    float d11 = mix(gridData(data, vx, vy+1, vz+1), gridData(data, vx+1, vy+1, vz+1), dx);
    float d0 = mix(d00, d10, dy);
    float d1 = mix(d01, d11, dy);
    return mix(d0, d1, dz);
}

vec3 hsvToRGB(vec3 in) {
    double      hh, p, q, t, ff;
    long        i;
    vec3        out;
    
    if(in[1] <= 0.0) {       // < is bogus, just shuts up warnings
        out.r = in[2];
        out.g = in[2];
        out.b = in[2];
        return out;
    }
    hh = in[0];
    if(hh >= 360.0) hh = 0.0;
    hh /= 60.0;
    i = (long)hh;
    ff = hh - i;
    p = in[2] * (1.0 - in[1]);
    q = in[2] * (1.0 - (in[1] * ff));
    t = in[2] * (1.0 - (in[1] * (1.0 - ff)));
    
    switch(i) {
        case 0:
            out.r = in[2];
            out.g = t;
            out.b = p;
            break;
        case 1:
            out.r = q;
            out.g = in[2];
            out.b = p;
            break;
        case 2:
            out.r = p;
            out.g = in[2];
            out.b = t;
            break;
            
        case 3:
            out.r = p;
            out.g = q;
            out.b = in[2];
            break;
        case 4:
            out.r = t;
            out.g = p;
            out.b = in[2];
            break;
        case 5:
        default:
            out.r = in[2];
            out.g = p;
            out.b = q;
            break;
    }
    return out;
}

float GridVolume::gridData(float* data, int x, int y, int z) const {
    if (!data) {
        return 0.f;
    }
    
    x = clamp(x, 0, (int)_sizeX-1);
    y = clamp(y, 0, (int)_sizeY-1);
    z = clamp(z, 0, (int)_sizeZ-1);
    return data[z*_sizeX*_sizeY + y*_sizeX + x];
}

Spectrum GridVolume::le(const vec3& p) const {
    Transform worldToObject = _parentNode ? Transform::Inverse(_parentNode->getTransform()) : Transform();
    float v = dataAtPoint(_temperature, worldToObject(p));
    float biais = -0.3f;
    v += biais;
    v = clamp(v, 0.f, 1.f);
    
    // Evaluate gradient
    vec3 color1 = vec3(15.f, 0.815f, 20.f/40.f);
    vec3 color2 = vec3(0.f, 0.f, 0.f);
    
    vec3 colorValue;
    
    if (v < 0.1429f) {
        colorValue = color1;
    } else if (v > 0.8571f) {
        colorValue = color2;
    } else {
        float t = (v - 0.1429f) / (0.8571f - 0.1429f);
        colorValue = glm::mix(color1, color2, t);
    }
    
    float d = dataAtPoint(_density, worldToObject(p));
    d *= 10.f;
    d = glm::clamp(d, 0.f, 1.f);
    float t1 = 0.35f, t2 = 0.8f;
    if (d < t1) {
        d = 0.f;
    } else if (d > t2) {
        d = 0.f;
    } else {
        float t = (d - t1) / (t2 - t1);
        d = glm::mix(0.f, 0.9f, t);
    }
    
    return d*0.1f * Spectrum(hsvToRGB(colorValue)).getColor()*1.f;
}