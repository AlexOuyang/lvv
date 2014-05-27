//
//  ImageLoading.h
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 5/8/14.
//
//

#ifndef __CSE168_Rendering__ImageLoader__
#define __CSE168_Rendering__ImageLoader__

#include "Core/Core.h"
#include "Core/Texture.h"

namespace ImageLoading {
    std::shared_ptr<Texture> LoadImage(const std::string& filename);
    std::shared_ptr<Texture> LoadFloatImage(const std::string& filename);
};

#endif /* defined(__CSE168_Rendering__ImageLoader__) */
