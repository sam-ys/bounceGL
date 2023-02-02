#include <memory>
#include <vector>

#include <glad/glad.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include "imgui/imgui.h"
#include "imguibackends/imgui_impl_opengl3.h"
#include "imguibackends/imgui_impl_sdl.h"

#include "images/awesome_face.h"
#include "images/brick_wall.h"
#include "images/incredulous_face.h"
#include "images/shocked_face.h"

#include "box.hpp"
#include "camera.hpp"
#include "draw_instanced_with_texture.hpp"
#include "draw_instanced_no_texture.hpp"
#include "grid_square.hpp"
#include "texture.hpp"

namespace {

    /*! Parameters that are initialized by SDL
     */
    struct SDLParam {
        SDL_Window* window;
        SDL_GLContext context;
    };

    /*! Initializes SDL
     */
    bool init_sdl(SDLParam& params, unsigned widthHint, unsigned heightHint)
    {
        // Initialize sdl
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
            return (printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError()), false);
        }

        // GL 3.0 + GLSL 130
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

        // Create window with graphics context
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
        SDL_WindowFlags flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
        params.window = SDL_CreateWindow("Bounce",
                                         SDL_WINDOWPOS_CENTERED,
                                         SDL_WINDOWPOS_CENTERED,
                                         widthHint,
                                         heightHint,
                                         flags);
        if (params.window == nullptr) {
            return (printf("Window could not be created! SDL Error: %s\n", SDL_GetError()), false);
        }

        // Create context
        params.context = SDL_GL_CreateContext(params.window);
        if (params.context == nullptr) {
            return (printf("OpenGL context could not be created! SDL Error: %s\n", SDL_GetError()), false);
        }

        SDL_GL_MakeCurrent(params.window, params.context);

        // Use Vsync
        if(SDL_GL_SetSwapInterval(1) != 0) {
            return (printf("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError()), false);
        }

        return true;
    }
}

namespace {

    //! struct BallData
    /*! Defines a moving ball
     */
    struct BallData {
        // Ball sprite index
        unsigned selectedSkin;
        // Ball direction
        calc::vec3f direction;
        // Ball speed
        calc::vec3f speed;
        // Ball turn rate
        calc::vec3f turnRate;
        // Current ball postition
        calc::mat4f translation;
        /*! ctor.
         */
        explicit BallData() : selectedSkin(0)
                            , direction(1.0, 1.0, 0)
                            , speed(0, 0, 0)
                            , translation(calc::mat4f::identity()) {}
    };
}

namespace {

    //! struct CtrlPanel
    /*! Defines a panel with game controls
     */
    struct CtrlPanel {

        SDL_Window* window;

        float pitch;
        float yaw;
        float roll;

        bool enableGrid;

        bool run;
        bool firstCall;

        float gridColour[3];
        float backgroundColour[3];

        /*! ctor.
         */
        explicit CtrlPanel(SDL_Window* window) : window(window)
                                               , pitch(0)
                                               , yaw(0)
                                               , roll(0)
                                               , enableGrid(true)
                                               , run(true)
                                               , firstCall(true) {
            backgroundColour[0] = 0.35;
            backgroundColour[1] = 0.45;
            backgroundColour[2] = 0.35;

            gridColour[0] = 0.75;
            gridColour[1] = 0.75;
            gridColour[2] = 0.75;
        }

        /*! Renders all ctrl panel
         */
        void render(BallData& refballData, Camera& refcamera, unsigned* skins, unsigned skinsCount) {

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
            render_box_subpanel(refballData, skins, skinsCount);

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
        void render_box_subpanel(BallData& refballData,
                                 unsigned* textures,
                                 unsigned  textureCount) {

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
        void stop(BallData& refballData) const {
            refballData.speed[0] = 0;
            refballData.speed[1] = 0;
            refballData.speed[2] = 0;
        }

        /*! Helper
         */
        void reset(BallData& refballData) const {
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
        void render_background_subpanel() {

            ImGui::Text("Background Properties");
            ImGui::Separator();

            // Background color control
            ImGui::ColorEdit3("Background color", backgroundColour);
            ImGui::Separator();

            // Grid color control
            ImGui::ColorEdit3("Grid color", gridColour);
            ImGui::Checkbox("Enable Grid", &enableGrid);
        }

        /*! Renders subpanel
         */
        void render_scene_subpanel(Camera& refcamera) {

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
        bool render_scene_angle_subpanel(Camera& refcamera) {

            ImGui::SliderFloat("Scene pitch angle", &pitch,    0.0f,  45.0f);
            ImGui::SliderFloat("Scene yaw angle",   &yaw,    -45.0f,  45.0f);
            ImGui::SliderFloat("Scene roll angle",  &roll,  -180.0f, 180.0f);

            float pitchRad = pitch;
            float yawRad = yaw;
            float rollRad = roll;

            if (std::abs(pitch - refcamera.get_pitch()) > 0.00001 ||
                std::abs(yaw - refcamera.get_yaw())     > 0.00001 ||
                std::abs(roll - refcamera.get_roll())   > 0.00001)
                return (refcamera.set_scene_rotation(pitchRad, yawRad, rollRad), true);
            // Nothing to do...
            return false;
        }

        /*! Renders subpanel
         */
        bool render_scene_position_subpanel(Camera& refcamera) {

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
    };
}

namespace{

    std::vector<calc::mat4f> build_grid(int width, int length)
    {
        static const float xdim = 1.0;
        static const float ydim = 1.0;

        calc::mat4f model = calc::mat4f::identity();
        float& x = model[3][0];
        float& y = model[3][1];

        const int xinst = std::ceil(width / 2.0 / xdim);
        const int yinst = std::ceil(length / 2.0 / ydim);

        const int size = xinst * yinst * 4;

        std::vector<calc::mat4f> grid;
        grid.resize(size);
        calc::mat4f* ptr = &grid[0];

        for (int i = -xinst; i != xinst; ++i)
        {
            for (int j = -yinst; j != yinst; ++j)
            {
                x = i * xdim + 0.5;
                y = j * ydim + 0.5;
                *ptr++ = model;
            }
        }

        return grid;
    }

    std::vector<calc::mat4f> build_wall(int width, int length)
    {
        std::vector<calc::mat4f> wall;

        // Padd dimensions
        width = width + (length % 2);
        length = length + (length % 2);

        // West wall
        for (int i = 1 - length / 2 / 3; i != length / 2 / 3; ++i)
        {
            calc::mat4f mat = calc::mat4f::identity();
            mat[0][0] = 3;
            mat[1][1] = 3;

            mat[0][3] = width / 2 - 1;
            mat[1][3] = i * 3;
            mat[2][3] = -1.0;
            wall.push_back(calc::transpose(mat));
        }

        // East wall
        for (int i = 1 - length / 2 / 3; i != length / 2 / 3; ++i)
        {
            calc::mat4f mat = calc::mat4f::identity();
            mat[0][0] = 3;
            mat[1][1] = 3;

            mat[0][3] = 1 - width / 2;
            mat[1][3] = i * 3;
            mat[2][3] = -1.0;
            wall.push_back(calc::transpose(mat));
        }

        // North wall
        for (int i = 1 - width / 2 / 3; i != width / 2 / 3; ++i)
        {
            calc::mat4f mat = calc::mat4f::identity();
            mat[0][0] = 3;
            mat[1][1] = 3;

            mat[0][3] = i * 3;
            mat[1][3] = length / 2 - 1;
            mat[2][3] = -1.0;
            wall.push_back(calc::transpose(mat));
        }

        // South wall
        for (int i = 1 - width / 2 / 3; i != width / 2 / 3; ++i)
        {
            calc::mat4f mat = calc::mat4f::identity();
            mat[0][0] = 3;
            mat[1][1] = 3;

            mat[0][3] = i * 3;
            mat[1][3] = 1 - length / 2;
            mat[2][3] = -1.0;
            wall.push_back(calc::transpose(mat));
        }

        return wall;
    }

    /*! Helper
     *! Converts matrix to float data
     */
    inline std::vector<float> copy_matrix_data(const std::vector<calc::mat4f>& mats)
    {
        std::vector<float> values(mats.size() * 16);
        for (::size_t i = 0; i != mats.size(); ++i)
            ::memcpy(&values[i * 16], calc::data(mats[i]), sizeof(calc::mat4f));
        return values;
    }
}

namespace {

    class Runner {

        // Control panel
        CtrlPanel                 panel_;

        std::shared_ptr<Camera>   camera_;
        std::shared_ptr<BallData> ballData_;

        DrawInstancedNoTexture    gridProgr_;
        DrawInstancedWithTexture  wallProgr_;

        render::Box               ballSkins_[3];
        render::Box               wallShape_;
        render::GridSquare        gridShape_;

        std::vector<unsigned>     textureHandles_;

        unsigned                  screenWidth_;
        unsigned                  screenHeight_;

        unsigned                  gridWidth_;
        unsigned                  gridLength_;

        unsigned                  cageWidth_;
        unsigned                  cageLength_;

        std::vector<float>        wall_;
        std::vector<float>        grid_;

    public:

        Runner(SDL_Window* window, Camera* camera) : panel_(window)
                                                   , camera_(camera)
                                                   , ballData_(new BallData()) {
            cageWidth_ = 30;
            cageLength_ = 30;

            gridWidth_ = 2 * cageWidth_;
            gridLength_ = 2 * cageLength_;

            grid_ = copy_matrix_data(build_grid(gridWidth_, gridLength_));
            wall_ = copy_matrix_data(build_wall(cageWidth_, cageLength_));

            unsigned ballTAO1[] = {
                render::load_texture_from_data(brick_wall_png, brick_wall_png_len),
                render::load_texture_from_data(awesome_face_png, awesome_face_png_len)
            };

            unsigned ballTAO2[] = {
                1,
                render::load_texture_from_data(shocked_face_png, shocked_face_png_len)
            };

            unsigned ballTAO3[] = {
                1,
                render::load_texture_from_data(incredulous_face_png, incredulous_face_png_len)
            };

            unsigned wallTAO[] = {
                1,
                1,
            };

            textureHandles_.push_back(render::load_texture_from_data(awesome_face_png, awesome_face_png_len, false));
            textureHandles_.push_back(render::load_texture_from_data(shocked_face_png, shocked_face_png_len, false));
            textureHandles_.push_back(render::load_texture_from_data(incredulous_face_png, incredulous_face_png_len, false));

            ballSkins_[0] = render::Box(ballTAO1, (sizeof(ballTAO1) / sizeof(unsigned)), 1);
            ballSkins_[0].push_back(calc::mat4f::identity());

            ballSkins_[1] = render::Box(ballTAO2, (sizeof(ballTAO2) / sizeof(unsigned)), 1);
            ballSkins_[1].push_back(calc::mat4f::identity());

            ballSkins_[2] = render::Box(ballTAO3, (sizeof(ballTAO3) / sizeof(unsigned)), 1);
            ballSkins_[2].push_back(calc::mat4f::identity());

            wallShape_ = render::Box(wallTAO, (sizeof(wallTAO) / sizeof(unsigned)), (cageWidth_ * cageLength_));
            gridShape_ = render::GridSquare((gridWidth_ * gridLength_));
        }

        /*! Evt. handler
         */
        void on_text_input(const SDL_Event&) {

            if ((ImGui::GetIO()).WantCaptureKeyboard) {
                return;
            }
        }

        /*! Evt. handler
         */
        void on_window_event(const SDL_Event& e) {

            if ((e.window).event == SDL_WINDOWEVENT_SIZE_CHANGED)
            {
                screenWidth_ = e.window.data1;
                screenHeight_ = e.window.data2;
                glViewport(0, 0, screenWidth_, screenHeight_);

                // Update camera
                Camera& refcamera = *camera_;
                refcamera.resize(screenWidth_, screenHeight_);
                refcamera.update();
            }
        }

        void run(const SDLParam& params) {

            while ((panel_.run))
            {
                //Handle events on queue
                SDL_Event e;
                while (SDL_PollEvent(&e) != 0)
                {
                    if (e.type == SDL_QUIT) {
                        return;
                    }

                    ImGui_ImplSDL2_ProcessEvent(&e);
                    switch (e.type)
                    {
                        case SDL_WINDOWEVENT:
                        {
                            on_window_event(e);
                            break;
                        }

                        // Handle keypress with current mouse position
                        case SDL_TEXTINPUT:
                        {
                            switch (e.text.text[0])
                            {
                                case SDLK_q: {
                                    return;
                                }

                                default:
                                {
                                    on_text_input(e);
                                    break;
                                }
                            }

                            break;
                        }
                    }
                }

                render(params);
            }
        }

        void render(const SDLParam& params) {

            glClearColor(panel_.backgroundColour[0],
                         panel_.backgroundColour[1],
                         panel_.backgroundColour[2],
                         1.0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            const calc::mat4f& lookAt     = camera_->get_device_look_at();
            const calc::mat4f& projection = camera_->get_device_projection();

            // Maybe draw the grid
            if (panel_.enableGrid)
            {
                gridProgr_.use();
                gridProgr_.set_colour(calc::vec4f(panel_.gridColour[0],
                                                  panel_.gridColour[1],
                                                  panel_.gridColour[2],
                                                  1.0));
                gridProgr_.set_scene(lookAt, projection);
                gridShape_.reset(grid_.data(), grid_.size() / 16);
                gridShape_.draw();
            }

            // Draw the wall
            wallProgr_.use();
            wallProgr_.set_scene(lookAt, projection);
            wallShape_.reset(wall_.data(), wall_.size() / 16);
            wallShape_.draw();

            // Draw the ball
            BallData& refballData = *ballData_;

            calc::vec3f& direction = refballData.direction;
            calc::vec3f& speed = refballData.speed;
            calc::mat4f& translation = refballData.translation;

            translation[2][3] = -1.0;
            float x = (translation[0][3] += speed[0] * direction[0]);
            float y = (translation[1][3] += speed[1] * direction[1]);

            // Bounce back on wall hit
            float hitOffset = 2.75;
            if (x < +hitOffset - (cageWidth_ / 2) ||
                x > -hitOffset + (cageWidth_ / 2)) {
                direction[0] *= -1;
            }

            if (y < +hitOffset - (cageLength_ / 2) ||
                y > -hitOffset + (cageLength_ / 2)) {
                direction[1] *= -1;
            }

            const calc::vec3f turnRate = refballData.turnRate * calc::radians(SDL_GetTicks() / 10.0);
            const calc::mat4f ballMat = calc::transpose(translation
                                                        * calc::rotate_4x(turnRate[0])
                                                        * calc::rotate_4y(turnRate[1])
                                                        * calc::rotate_4z(turnRate[2]));

            render::Box& refshape = ballSkins_[refballData.selectedSkin];
            refshape.modify(calc::data(ballMat), 0);
            refshape.draw();

            // Draw the control panel
            panel_.render(refballData, *camera_, textureHandles_.data(), textureHandles_.size());
            // Update screen & return
            SDL_GL_SwapWindow(params.window);
        }
    };
}

/*! Entry point
 */
int main(void)
{
    const unsigned screenWidth = 100;
    const unsigned screenHeight = 100;

    //Initialize SDL
    SDLParam params;
    if (!init_sdl(params, screenWidth, screenHeight))
    {
        printf("Error initializing SDL\n");
        return 1;
    }

    // Load all OpenGL functions using the SDL loader function
    if (gladLoadGLLoader(reinterpret_cast<GLADloadproc>(SDL_GL_GetProcAddress)))
        glEnable(GL_DEPTH_TEST);
    else
    {
        printf("Error initializing OpenGL\n");
        return 1;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
    io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;
    io.BackendFlags |= ~ImGuiBackendFlags_HasMouseHoveredViewport;

    ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForOpenGL(params.window, params.context);
    ImGui_ImplOpenGL3_Init("#version 130");

    // Init camera defaults
    static float xPos = 0;
    static float yPos = 0;
    static float zPos = -40.0;
    static float fov  = 30.0;
    static float zFar = 1000.0;

    Runner runner(params.window, new Camera(calc::vec3f(xPos, yPos, zPos), fov, zFar));
    runner.run(params);

    SDL_StopTextInput();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    return 0;
}
