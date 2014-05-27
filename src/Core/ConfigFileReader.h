//
//  ConfigFileReader.h
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 5/25/14.
//
//

#ifndef __CSE168_Rendering__ConfigFileReader__
#define __CSE168_Rendering__ConfigFileReader__

#include "Core/Core.h"
#include "Core/Renderer.h"
#include "Core/Scene.h"

#include <map>

class ConfigFileReader {
public:
    
    ConfigFileReader();
    ~ConfigFileReader();
    
    bool readFile(const std::string& filename);
    
private:
    std::shared_ptr<Renderer>   _renderer;
    std::shared_ptr<Scene>      _scene;
};

#endif /* defined(__CSE168_Rendering__ConfigFileReader__) */
