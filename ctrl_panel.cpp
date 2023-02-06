#include <SDL2/SDL.h>
#include <SDL2/SDL_opengles.h>

#include "dear_imgui/imgui.h"
#include "dear_imgui_backends/imgui_impl_opengl3.h"
#include "dear_imgui_backends/imgui_impl_sdl.h"

#include "ball_data.hpp"
#include "camera.hpp"
#include "ctrl_panel.hpp"

/*! ctor.
 */
CtrlPanel::CtrlPanel(SDL_Window* window) : window(window)
                                         , pitch(0)
                                         , yaw(0)
                                         , roll(0)
                                         , enableGrid(true)
                                         , run(true)
                                         , firstCall(true)
{
    backgroundColor[0] = 0.35;
    backgroundColor[1] = 0.45;
    backgroundColor[2] = 0.35;

    gridColor[0] = 0.75;
    gridColor[1] = 0.75;
    gridColor[2] = 0.75;
}

/*! Renders all ctrl panel
 */
void CtrlPanel::render(BallData& refballData, Camera& refcamera, unsigned* skinHandles, unsigned skinHandlesCount)
{
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(window);

    ImGui::NewFrame();
    ImGui::Begin("Control Panel");

    if (firstCall)
    {
        firstCall = false;
        ImGui::SetWindowSize("Control Panel", ImVec2(600, 700));
    }

    // Box...
    render_ball_subpanel(refballData, skinHandles, skinHandlesCount);

    ImGui::Separator();
    ImGui::Dummy(ImVec2(0, 30));

    // Background...
    render_background_subpanel();

    ImGui::Separator();
    ImGui::Dummy(ImVec2(0, 30));

    // Scene...
    render_scene_subpanel(refcamera);

    ImGui::End();

    // Render imgui
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

/*! Renders subpanel
 */
void CtrlPanel::render_ball_subpanel(BallData& refballData, unsigned* textures, unsigned  textureCount)
{
    ImGui::Text("Box Properties");
    ImGui::Separator();

    // Control group
    for (::size_t i = 0; i != textureCount; ++i)
    {
        unsigned long k = textures[i];
        if (ImGui::ImageButton((void*)k, ImVec2(100, 100)))
            refballData.selectedSkin = i;
        if (i != textureCount - 1)
            ImGui::SameLine();
    }

    ImGui::Separator();

    // Control group
    ImGui::SliderFloat("Box x-speed", &refballData.speed[0], 0.0f, 0.2f);
    ImGui::SliderFloat("Box y-speed", &refballData.speed[1], 0.0f, 0.2f);
    ImGui::Separator();

    // Control group
    ImGui::SliderFloat("Box x-axis turn rate", &refballData.turnRate[0], 0.0f, 2.5f);
    ImGui::SliderFloat("Box y-axis turn rate", &refballData.turnRate[1], 0.0f, 2.5f);
    ImGui::SliderFloat("Box z-axis turn rate", &refballData.turnRate[2], 0.0f, 2.5f);
    ImGui::Separator();

    // Control group
    if (ImGui::Button("Stop Box"))
        stop(refballData);
    ImGui::SameLine();
    if (ImGui::Button("Reset Box"))
        reset(refballData);
}

/*! Helper
 */
void CtrlPanel::stop(BallData& refballData) const
{
    refballData.speed[0] = 0;
    refballData.speed[1] = 0;
    refballData.speed[2] = 0;
}

/*! Helper
 */
void CtrlPanel::reset(BallData& refballData) const
{
    stop(refballData);
    refballData.turnRate[0] = 0;
    refballData.turnRate[1] = 0;
    refballData.turnRate[2] = 0;

    refballData.translation[0][3] = 0;
    refballData.translation[1][3] = 0;
    refballData.direction[0] = 1.0;
    refballData.direction[1] = 1.0;
}

/*! Renders subpanel
 */
void CtrlPanel::render_background_subpanel()
{
    ImGui::Text("Background Properties");
    ImGui::Separator();

    // Background color control
    ImGui::ColorEdit3("Background color", backgroundColor);
    ImGui::Separator();

    // Grid color control
    ImGui::ColorEdit3("Grid color", gridColor);
    ImGui::Checkbox("Enable Grid", &enableGrid);
}

/*! Renders subpanel
 */
void CtrlPanel::render_scene_subpanel(Camera& refcamera) {

    ImGui::Text("Scene Properties");
    ImGui::Separator();

    // Scene angle...
    bool update;
    update = render_scene_angle_subpanel(refcamera);
    ImGui::Separator();

    // Camera position...
    update |= render_scene_position_subpanel(refcamera);
    ImGui::Separator();

    // Reset
    if (ImGui::Button("Reset Scene"))
    {
        update = true;

        refcamera.reset();
        pitch = refcamera.get_pitch();
        yaw = refcamera.get_yaw();
        roll = refcamera.get_roll();
    }

    // Maybe update camera
    if (update)
        refcamera.update();

    // Exit...
    ImGui::Separator();
    ImGui::Dummy(ImVec2(540, 0));
    ImGui::SameLine();
    if (ImGui::Button("Exit")) {
        run = false;
    }
}

/*! Renders subpanel
 */
bool CtrlPanel::render_scene_angle_subpanel(Camera& refcamera)
{
    pitch = refcamera.get_pitch();
    yaw = refcamera.get_yaw();
    roll = refcamera.get_roll();

    ImGui::SliderFloat("Scene pitch angle", &pitch,    0.0f,  45.0f);
    ImGui::SliderFloat("Scene yaw angle",   &yaw,    -45.0f,  45.0f);
    ImGui::SliderFloat("Scene roll angle",  &roll,  -180.0f, 180.0f);

    if (std::abs(pitch - refcamera.get_pitch()) > 0.00001 ||
        std::abs(yaw - refcamera.get_yaw())     > 0.00001 ||
        std::abs(roll - refcamera.get_roll())   > 0.00001)
        return (refcamera.set_scene_rotation(pitch, yaw, roll), true);
    // Nothing to do...
    return false;
}

/*! Renders subpanel
 */
bool CtrlPanel::render_scene_position_subpanel(Camera& refcamera)
{
    calc::vec3f position = ([&refcamera]() {
        calc::vec3f value = refcamera.get_position();
        value[2] = -value[2];
        return value;
    }());

    ImGui::SliderFloat("Viewer x-position", &position[0], -10, 10);
    ImGui::SliderFloat("Viewer y-position", &position[1], -10, 10);
    ImGui::SliderFloat("Viewer z-position", &position[2],  10, 40);

    calc::vec3f correctedPosition = position;
    correctedPosition[2] = -position[2];

    if (refcamera.get_position() != correctedPosition)
        return (refcamera.set_position(correctedPosition), true);
    // Nothing to do...
    return false;
}
