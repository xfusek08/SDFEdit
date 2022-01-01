
#pragma once

#include <data/Scene.h>

class VisualizationTechnique
{
    public:
        virtual void init(std::shared_ptr<Scene> scene) {}
        virtual void prepare(Scene& scene) {}
        virtual void render(Scene& scene) = 0;
};

using VTArray = std::vector<std::shared_ptr<VisualizationTechnique>>;
