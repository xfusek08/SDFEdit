
#include <systems/RandomMovementTestBench.h>
#include <RenderBase/tools/random.h>

using namespace std;

RandomMovementTestBench::MotionData RandomMovementTestBench::MotionData::random()
{
    return {
        rb::random::randomVec3({0.007, 0.007, 0.007}, {0.02, 0.02, 0.02}),
        rb::random::randomVec3({0.01, 0.01, 0.01}, {0.04, 0.04, 0.04}),
    };
}

void RandomMovementTestBench::init(std::shared_ptr<Scene> scene)
{
    editMovements = {};
    if (scene->models.size() > 0) {
        auto geometry = scene->models[0].geometry;
        editMovements.reserve(geometry->getEdits().size());
        for (const auto& edit : geometry->getEdits()) {
            editMovements.push_back(MotionData::random());
        }
    }
}

void RandomMovementTestBench::onInputChange(std::shared_ptr<Scene> scene, const rb::input::InputState& input, const rb::timing::TimeStep& tick)
{
    
}

void RandomMovementTestBench::onTick(std::shared_ptr<Scene> scene, const rb::input::InputState& input, const rb::timing::TimeStep& tick)
{
    if (!scene->hasActiveFlag("run_random_movement")) return;
    
    if (editMovements.size() > 0) {
        auto  geometry = scene->models[0].geometry;
        auto& edits    = geometry->getEdits();
        for (int i = 0; i < edits.size(); ++i) {
            edits[i].transform.position = edits[i].transform.position + editMovements[i].velocity;
            edits[i].transform.orientation = edits[i].transform.orientation * glm::quat(editMovements[i].eulerRotation);
            
            
            #define BOUNCE_BOUNDARIES(coord) \
                if ((editMovements[i].velocity.coord < 0 && (edits[i].transform.position.coord < boundarries.min.coord)) || \
                    (editMovements[i].velocity.coord > 0 && (edits[i].transform.position.coord > boundarries.max.coord)) \
                ) editMovements[i].velocity.coord *= -1
            
            BOUNCE_BOUNDARIES(x);
            BOUNCE_BOUNDARIES(y);
            BOUNCE_BOUNDARIES(z);
        }
        geometry->updateAABB();
        evaluator->addToEvaluation(geometry);
        
        scene->models[0].transform.position = geometry->getAABB().center();
    }
}
