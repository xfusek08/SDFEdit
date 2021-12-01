
#pragma once

#include <data/Scene.h>

class VisualizationTechnique
{
    public:
        virtual void prepare(const Scene& scene) = 0;
        virtual void render(const Scene& scene) = 0;
};

using VTArray = std::vector<std::shared_ptr<VisualizationTechnique>>;
