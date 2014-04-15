//
//  VisibilityTester.h
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/5/14.
//
//

#ifndef __CSE168_Rendering__VisibilityTester__
#define __CSE168_Rendering__VisibilityTester__

#include "Ray.h"

class Scene;

class VisibilityTester {
public:
    
    VisibilityTester();
    ~VisibilityTester();
    
    bool unoccluded(const Scene& scene) const;
    
    void setSegment(const vec3& p1, float epsilon, const vec3& p2);
    void setRay(const vec3& origin, float epsilon, const vec3& direction);
    
private:
    Ray _ray;
};

#endif /* defined(__CSE168_Rendering__VisibilityTester__) */
