
#include <Gui.h>

#include <imguiVars/imguiVars.h>
#include <imguiVars/addVarsLimits.h>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <GLFW/glfw3.h>

using namespace std;
using namespace rb;

const char* glsl_version = "#version 460";

Gui::Gui(const window::Window& window)
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    
    ImGui_ImplGlfw_InitForOpenGL(window.getGLFWHandle(), true);
    ImGui_ImplOpenGL3_Init(glsl_version);
    
    // styling move to custom method to be overriden
    ImGui::StyleColorsDark();
    // fonts?
}

Gui::~Gui()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void Gui::init(shared_ptr<Scene> scene)
{
    #define defvar(ident) \
        scene->vars.addFloat(#ident, ident); \
        addVarsLimitsF(scene->vars, #ident, -15.0, 15.0, 0.001)
        
    defvar(a);
    defvar(b);
    defvar(c);
    defvar(d);
}

void Gui::prepare(shared_ptr<Scene> scene)
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // bool showDemo = true;
    // ImGui::ShowDemoWindow(&showDemo);
    drawImguiVars(scene->vars);
    
    ImGui::Render();
}

void Gui::render(const Scene& scene)
{
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
