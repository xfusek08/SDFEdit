#pragma once

#include <systems/System.h>
#include <systems/GeometryEvaluator.h>
#include <glm/glm.hpp>
#include <data/AABB.h>

/**
 * This class randomly moves all edits in first geometry of the scene for performance testing purposes
 */
class RandomMovementTestBench : public System
{
    public:
        AABB boundarries = {};
        RandomMovementTestBench(AABB boundarries, std::shared_ptr<GeometryEvaluator> evaluator) : boundarries(boundarries), evaluator(evaluator) {}
        
        void init(std::shared_ptr<Scene> scene) override;
        void onInputChange(std::shared_ptr<Scene> scene, const rb::input::InputState& input, const rb::timing::TimeStep& tick) override;
        void onTick(std::shared_ptr<Scene> scene, const rb::input::InputState& input, const rb::timing::TimeStep& tick) override;
        
    private:
        struct MotionData {
            glm::vec3 velocity;
            glm::vec3 eulerRotation;
            static MotionData random();
        };
        
        std::vector<MotionData> editMovements = {};
        std::shared_ptr<GeometryEvaluator> evaluator = nullptr;
};
