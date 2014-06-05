//
//  Aggregate.h
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/15/14.
//
//

#ifndef __CSE168_Rendering__Aggregate__
#define __CSE168_Rendering__Aggregate__

#include "Core.h"
#include "Primitive.h"

class Aggregate : public Primitive {
public:
    
    virtual ~Aggregate();
    
    Aggregate& operator<<(const std::shared_ptr<Primitive>& primitive);
    
    virtual void addPrimitive(const std::shared_ptr<Primitive>& primitive) = 0;
    virtual void preprocess();
    virtual void rebuild();
    
    virtual std::shared_ptr<Primitive> findPrimitive(const std::string& name);
    virtual void removePrimitive(const std::string& name);
    virtual const std::vector<std::shared_ptr<Primitive>> getPrimitives() const;
};

#endif /* defined(__CSE168_Rendering__Aggregate__) */
