
#include <components/renderer.h>

using namespace std;
using namespace rb;

Renderer::Renderer(vector<shared_ptr<VisualizationTechnique>> vts) : vts(vts)
{
    glClearColor(0.2, 0.3, 0.7, 1);
}

void Renderer::render(const AppState& appState)
{
    // Preparation is non-blocking and independent for all vts
    // thus this will dispatch all preparations
    // TODO: maybe run each VT in its own thread and then wait for them all after the loop.
    
    for (auto& vt : vts) {
        vt->prepare(appState);
    }
    
    // Rendering -> each vt renders itself and performs a blocking wating for its preparation process
    glClear(GL_COLOR_BUFFER_BIT);
    for (auto& vt : vts) {
        vt->render(appState);
    }
}
