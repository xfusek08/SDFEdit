#pragma once

#include <visualization/VisualizationTechnique.h>

/**
 * This class will take care of rendering all models which has link to a geometry
 */
class ModelVT : VisualizationTechnique
{
    /**
     * This function will evaluate all changed geometries and
     */
    void prepare(const Scene& scene) override;
    void render(const Scene& scene) override;
};
