#pragma once

#include <types/scene.h>

#include <components/ElementPoolBehavior.h>

struct ModelPool : public ItemPoolBehavior<Model> {
    
    // TODO: manipulation with models will be guarder to maintail track of changes which will be load to gpu model counter parts
    
    ModelPool() {}
        
    ~ModelPool() {}
    
    private:
        // TODO: + models will be tightly packed into continuous buffer and managend by this poll, which will track changes of the models and updates them to the gpu
};
