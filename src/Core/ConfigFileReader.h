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
#include "Core/Film.h"

class ConfigFileReader {
public:
    
    ConfigFileReader();
    ~ConfigFileReader();
    
    bool readFile(const std::string& filename);
    
    std::shared_ptr<Film> getFilm() const;
    std::shared_ptr<Renderer> getRenderer() const;
    std::shared_ptr<Scene> getScene() const;
    
    // Utility functions
    static bool LoadFileContents(std::string filename, std::string& contents);
    static vec3 LoadVec3(const rapidjson::Value& value);
    
private:
    std::shared_ptr<Film>       _film;
    std::shared_ptr<Renderer>   _renderer;
    std::shared_ptr<Scene>      _scene;
};

#endif /* defined(__CSE168_Rendering__ConfigFileReader__) */
