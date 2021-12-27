
#include <systems/GeometryEvaluator.h>

#include <unordered_set>

using namespace std;
using namespace rb;

// static settings
#define BRANCHING_FACTOR 2
#define STRINGIFY2(X) #X
#define STRINGIFY(X) STRINGIFY2(X)
#define SHADER_MACROS gl::ShaderDefines{"DEBUG", "BRANCHING_FACTOR " STRINGIFY(BRANCHING_FACTOR)}

struct GPUEdit {
    uint32    type;
    uint32    op;
    float32   blending;
    uint32    _padding1;
    glm::mat4 transform;
    glm::vec4 data;
};

GeometryEvaluator::GeometryEvaluator() :
    octreeEvaluationProgram(
        make_shared<gl::Shader>(GL_COMPUTE_SHADER, RESOURCE_SHADERS_VSO_EVALUATE_BRICK_COMP, SHADER_MACROS)
    ),
    octreeInitiationProgram(
        make_shared<gl::Shader>(GL_COMPUTE_SHADER, RESOURCE_SHADERS_VSO_INIT_NEW_LEVEL_NODES_COMP, SHADER_MACROS)
    )
{
    editBuffer    = make_unique<gl::Buffer>(sizeof(GPUEdit) * 100, nullptr, GL_DYNAMIC_DRAW);
    counterBuffer = make_unique<gl::Buffer>(sizeof(uint32) * 2, nullptr, GL_DYNAMIC_DRAW);
}

void GeometryEvaluator::init(std::shared_ptr<Scene> scene)
{
    for (const auto& model : scene->models) {
        AddToEvaluation(model.geometry);
    }
    
    scene->vars.setChangeCallback("blending", [this, scene]() {
        auto geometry = scene->models[0].geometry;
         for (auto& edit : geometry->getEdits()) {
            edit.blending = scene->vars.getFloat("blending");
         }
        AddToEvaluation(geometry);
    });
    
    scene->vars.setChangeCallback("rounding", [this, scene]() {
        auto geometry = scene->models[0].geometry;
        for (auto& edit : geometry->getEdits()) {
            if (edit.primitiveType == PrimitiveType::ptBox) {
                edit.primitiveData.w = scene->vars.getFloat("rounding");
            } else if (edit.primitiveType == PrimitiveType::ptCylinder) {
                edit.primitiveData.z = scene->vars.getFloat("rounding");
            } else if (edit.primitiveType == PrimitiveType::ptCone) {
                edit.primitiveData.w = scene->vars.getFloat("rounding");
            }
        }
        AddToEvaluation(geometry);
    });
    
    // scene->vars.setChangeCallback("blending", [this, scene]() {
    //     auto geometry = scene->models[0].geometry;
    //     auto blending = scene->vars.getFloat("blending");
    //     geometry->getEdit(0).blending = blending;
    //     AddToEvaluation(geometry);
    // });
    
    // scene->vars.setChangeCallback("d", [this, scene]() {
    //     AddToEvaluation(scene->models[0].geometry);
    // });
}

void GeometryEvaluator::onInputChange(shared_ptr<Scene> scene, const rb::input::InputState& input, const rb::timing::TimeStep& tick)
{
    if (input.isKeyPressed(GLFW_KEY_R)) {
        init(scene);
    }
}

void GeometryEvaluator::onTick(shared_ptr<Scene> scene, const rb::input::InputState& input, const rb::timing::TimeStep& tick)
{
    auto setVar = [&] (string ident) {
        float val = scene->vars.getFloat(ident);
        octreeEvaluationProgram.uniform(ident.c_str(), val);
    };
    setVar("a");
    setVar("b");
    setVar("c");
    setVar("d");
    
    evaluateQueue();
}

void GeometryEvaluator::evaluateQueue()
{
    for (auto geometry : toEvaluateQueue) {
        geometry->octree = evaluateGeometry(*geometry);
    }
    toEvaluateQueue.clear();
}

std::shared_ptr<SVOctree> GeometryEvaluator::evaluateGeometry(const Geometry& geometry) const
{
    // prepare data and buffers for shaders and the algorithm
    // -------------------------------------
    
    // init octree data structure which will be filled by following algorithm
    auto octree = make_shared<SVOctree>(BRANCHING_FACTOR, 10000);
    
    // init first level for algorithm runtime
    SVOctree::Level* currentLevel = octree->initFirstLevel();

    // prepare evaluator input -> list opf edits
    loadEditBuffer(geometry);
    
    // prepare atomic counter buffer
    uint32 counters[2] = { uint32(octree->nodeCount), uint32(octree->brickPool->brickCount) };
    counterBuffer->setData(counters, sizeof(uint32) * 2);
    
    // bind all buffers
    counterBuffer->bindBase(GL_ATOMIC_COUNTER_BUFFER, 0);
    octree->nodeBuffer->bindBase(GL_SHADER_STORAGE_BUFFER, 1);
    octree->nodeDataBuffer->bindBase(GL_SHADER_STORAGE_BUFFER, 2);
    octree->vertexBuffer->bindBase(GL_SHADER_STORAGE_BUFFER, 3);
    editBuffer->bindBase(GL_SHADER_STORAGE_BUFFER, 4);
    octree->brickPool->bind(1, GL_WRITE_ONLY);

    // uniform: shift and scale SVO to be aligned with BB of the geometry
    const glm::vec4 correctionVector = glm::vec4(geometry.getAABB().center(), geometry.getAABB().longestEdgeSize());
    octreeEvaluationProgram.uniform("correctionVector", correctionVector);
    octreeEvaluationProgram.uniform("editCount", uint32(geometry.readEdits().size()));
    
    octreeInitiationProgram.uniform("correctionVector", correctionVector);
    
    // Run evaluation algorithm
    // -------------------------
    
    auto resizeBuffer = [](gl::Buffer& buffer, uint32 requestedSize) {
        uint32 origSize = buffer.getSize();
        uint32 newSize = origSize;
        while(requestedSize > newSize) {
            newSize *= 1.5;
        }
        if (newSize != origSize) {
            RB_DEBUG("Buffer " << buffer.getGlID() << " reallocation: " << origSize << "->" << newSize);
            buffer.resize(newSize);
        }
    };
    
    // octree->debugPrint();
    
    // 1. Run init kernel to initialize root tile:
    //   - Current level is not initialized.
    //   - Kernel is dispatched only for current root tile with special flag to init first level with predefined values.

    octreeInitiationProgram.uniform("levelBeginIndex", currentLevel->startNode); // index of beginding of current level
    octreeInitiationProgram.uniform("initRootTile", 1u); // tells kernel to use default vertex positions for root node tile
    octreeInitiationProgram.use();
    glDispatchCompute(1, 1, 1); // dispatch one workgroup for root tile
    octreeInitiationProgram.uniform("initRootTile", 0u); // all following dispatches will be general for current level
    
    // octree->debugPrint();
    
    do {
        
        // 2. Run evaluation kernel
        //   - Current SVO level is initialized and each tile is zeroed and has defined its position.
        //   - Spawns a workgroup for each node in current level which will evaluate SDF values into for a brick in place of the node.
        //   - Each node is evaluated using 8x8x8 thrreads one for each voxel in the volume.
        //   - These threads will calculate and store distances an base on that they will vote to subdivide the node or not to.
        //   - If at least one voxel intersects volume surface, the node will get subdivided.
        //   - The subdivision will be done by one thread in workgroup by incrementing node counter to create a new tile for divided node.
        //   - The new tile will be uninitialized prepared for initiation kernel to compute verticies for newly created nodes.
        //   - If node was subdivided then evaluated volume is stored into brick atlas and its coordinates are stored into a node data buffer.
        //   - If node was not subdivided it is marked as 'full' or 'empty' inside node data buffer.
        
        // 2.1. dispatch kernel
        octreeEvaluationProgram.uniform("levelBeginIndex", currentLevel->startNode);
        octreeEvaluationProgram.uniform("allowSubdivision", uint32(currentLevel->depth < maxSubdivisions));
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT); // wait till previous initializedion kernel finishes writing values.
        octreeEvaluationProgram.use();
        RB_DEBUG("Dispatching evaluation compute: " << currentLevel->nodeCount);
        glDispatchCompute(currentLevel->nodeCount, 1, 1);

        // 2.2. Retrieve values from atomic counters and calculate differences
        glMemoryBarrier(GL_ATOMIC_COUNTER_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT | GL_SHADER_IMAGE_ACCESS_BARRIER_BIT); // wait till all computation is done
        uint32 counterData[2];
        counterBuffer->getData(&counterData, 2 * sizeof(uint32));
        
        // 2.3. update brick count in current level
        currentLevel->bricksInLevel = counterData[1] - octree->brickPool->brickCount;
        
        // 2.4. update current global counter values
        octree->nodeCount             = counterData[0]; // store info about size of the next level
        octree->brickPool->brickCount = counterData[1]; // we can update global brick count right away
        
        // 2.5. calculate next level values
        SVOctree::Level nextLevel = {};
        nextLevel.startNode = (currentLevel->startNode + currentLevel->nodeCount);
        nextLevel.nodeCount = octree->nodeCount - nextLevel.startNode;
        
        resizeBuffer(*octree->nodeBuffer, sizeof(uint32) * octree->nodeCount);
        resizeBuffer(*octree->nodeDataBuffer, sizeof(uint32) * octree->nodeCount);
        resizeBuffer(*octree->vertexBuffer, sizeof(glm::vec4) * octree->nodeCount);
                
        // octree->debugPrint();
        
        // 3. next level is empty -> we are done
        if (nextLevel.nodeCount == 0) {
            break;
        }
            
        // 4. If some new node tiles were spawn, run initiation kernel for current level to compute positions for the new nodes.
        //   - Current level is initialized and new uninitialized node tiles in next level are prepared.
        //   - Spawns a workgroup for each tile in current level with thread for each children in child node tile.
        //   - If such child node tile exits, each node is zeroed by its thread and computed a vertex position based on position of the parent.
        
        // 4.1 dispatch kernel
        octreeInitiationProgram.uniform("levelBeginIndex", currentLevel->startNode);
        octreeInitiationProgram.use();
        glDispatchCompute(currentLevel->nodeCount, 1, 1);
        
        // 5. move current level of the octree to the next newly generated one
        nextLevel.depth = currentLevel->depth + 1;
        currentLevel = octree->addLevel(nextLevel);
        
        // octree->debugPrint();
        // octree->brickPool->debugDraw();
        
    } while(true); // no new nodes means algorithm is finished
    
    // octree->debugPrint();
    // octree->debugPrintLevels();
    
    RB_DEBUG("Geometry evaluated into: " << octree->nodeCount << " nodes and " << octree->brickPool->brickCount << " bricks");
    
    // save evaluated octree to the geometry
    return octree;
}

/**
 * Load geometry edit list to gpu
 * ------------------------------
 * this is dirty solution to actually store edit list in geometry in such way to be
 * directly loadable to gpu without transforming
 */
void GeometryEvaluator::loadEditBuffer(const Geometry& geometry) const
{
    std::vector<GPUEdit> editsData;
    editsData.reserve(geometry.readEdits().size());
    for (auto e : geometry.readEdits()) {
        editsData.push_back({
            e.primitiveType,
            e.operation,
            e.blending,
            0,
            e.transform.getTransform(),
            e.primitiveData
        });
    }
    editBuffer->setData(editsData);
}
