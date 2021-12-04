
#include <Renderer.h>

using namespace std;
using namespace rb;

Renderer::Renderer(vector<shared_ptr<VisualizationTechnique>> vts) : vts(vts)
{
    glClearColor(0.2, 0.3, 0.7, 1);
}

void Renderer::init(shared_ptr<Scene> scene)
{
    for (auto vt : vts) {
        vt->init(scene);
    }
}

void Renderer::prepare(const Scene& scene)
{
    for (auto vt : vts) {
        vt->prepare(scene);
    }
    // TODO: this could be done in parallel if concurent gl calls are allowed over one context
    // if not separate context is needed for each VT to be evaluated concurrently or final gl calls will be in
    // under a mutex.
}

void Renderer::render(const Scene& scene)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    for (auto vt : vts) {
        vt->render(scene);
    }
}