#include <memory>
#include <vector>

#include <glad/glad.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include "imgui/imgui.h"
#include "imguibackends/imgui_impl_opengl3.h"
#include "imguibackends/imgui_impl_sdl.h"

#include "box.hpp"
#include "camera.hpp"
#include "draw_instanced_with_texture.hpp"
#include "texture.hpp"

namespace {

    struct SDLParam {
        SDL_Window* window;
        SDL_GLContext context;
    };

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
        // Rendered object
        const std::shared_ptr<render::box> shape;
        // Ball direction
        calc::vec3f direction;
        // Ball speed
        calc::vec3f speed;
        // Current ball postition
        calc::mat4f translation;
        // ctor.
        explicit BallData(render::box* ptr) : shape(ptr)
                                            , direction(1.0, 1.0, 0)
                                            , speed(0.08, 0.02, 0)
                                            , translation(calc::mat4f::identity()) {}
    };
}

namespace {

    //! struct CtrlPanel
    /*! Defines a control panel
     */
    struct CtrlPanel {

        SDL_Window* window;

        float backgroundColor[3];

        explicit CtrlPanel(SDL_Window* window) : window(window) {
            backgroundColor[0] = 0.35;
            backgroundColor[1] = 0.45;
            backgroundColor[2] = 0.35;
        }

        void render(BallData& refballData, Camera& refcamera) {

            // Start the Dear ImGui frame
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplSDL2_NewFrame(window);

            ImGui::NewFrame();

            ImGui::Begin("Control Panel");

            // Speed controls
            static float vx = 0.0f;
            ImGui::SliderFloat("Ball x-speed", &vx, 0.0f, 0.2f);
            refballData.speed[0] = vx;

            static float vy = 0.0f;
            ImGui::SliderFloat("Ball y-speed", &vy, 0.0f, 0.2f);
            refballData.speed[1] = vy;
            ImGui::Separator();

            // Background color control
            ImGui::ColorEdit3("Background color", backgroundColor);
            ImGui::Separator();

            // Camera angle
            static float pitchAngle = 0.0f, yawAngle = 0.0f, rollAngle = 0.0f;
            ImGui::SliderFloat("Camera pitch angle", &pitchAngle, 0.0f, 45.0f);
            ImGui::SliderFloat("Camera yaw angle",   &yawAngle,   0.0f, 45.0f);
            ImGui::SliderFloat("Camera roll angle",  &rollAngle,  0.0f, 45.0f);

            float pitchAngleRad = (pitchAngle);
            float yawAngleRad = (yawAngle);
            float rollAngleRad = (rollAngle);

            if (std::abs(pitchAngle - refcamera.get_pitch()) > 0.00001 ||
                std::abs(yawAngle - refcamera.get_yaw()) > 0.00001 ||
                std::abs(rollAngle - refcamera.get_roll()) > 0.00001)
            {
                refcamera.set_scene_rotation(pitchAngleRad, yawAngleRad, rollAngleRad);
                refcamera.update();
            }

            ImGui::End();

            // Render imgui
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }
    };
}

namespace {

    class Runner {

        unsigned screenWidth_;
        unsigned screenHeight_;

        CtrlPanel panel_;

        std::shared_ptr<Camera> camera_;
        std::shared_ptr<draw_instanced_with_texture> draw_;
        std::shared_ptr<BallData> ballData_;

    public:

        Runner(SDL_Window* window, Camera*&& camera) : panel_(window)
                                                     , camera_(camera)
                                                     , draw_(new draw_instanced_with_texture()) {
            camera = nullptr;
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

        void set_data(std::shared_ptr<BallData> data) {
            ballData_ = data;
        }

        void run(const SDLParam& params) {

            while (true)
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

            glClearColor(panel_.backgroundColor[0],
                         panel_.backgroundColor[1],
                         panel_.backgroundColor[2],
                         1.0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            Camera& refcamera = *camera_;
            const calc::mat4f& lookAt = refcamera.get_device_look_at();
            const calc::mat4f& projection = refcamera.get_device_projection();

            draw_instanced_with_texture& refdraw = *draw_;
            refdraw.use();
            refdraw.set_scene(lookAt, projection);

            std::vector<calc::mat4f> mats;

            const int n = 14;

            for (int i = -(n / 2.0); i != (n / 2.0); ++i)
            {
                calc::mat4f m = calc::mat4f::identity();
                m[0][3] = n / 2 - 1;
                m[1][3] = i;
                mats.push_back(m);

                if (i % 3 == 0 || i % 2 == 0)
                {
                    m[2][3] = -1;
                    mats.push_back(m);
                }
            }

            for (int i = -(n / 2.0); i != (n / 2.0); ++i)
            {
                calc::mat4f m = calc::mat4f::identity();
                m[0][3] = -(n / 2 - 1);
                m[1][3] = i;
                mats.push_back(m);

                if (i % 4 == 0 || i % 5 == 0)
                {
                    m[2][3] = -1;
                    mats.push_back(m);
                }
            }

            for (int i = 1 - (n / 2.0); i != (n / 2.0) - 1; ++i)
            {
                calc::mat4f m = calc::mat4f::identity();
                m[0][3] = i;
                m[1][3] = n / 2 - 1;
                mats.push_back(m);
            }

            for (int i = 1 - (n / 2.0); i != (n / 2.0) - 1; ++i)
            {
                calc::mat4f m = calc::mat4f::identity();
                m[0][3] = i;
                m[1][3] = -(n / 2 - 1);
                mats.push_back(m);
            }

            BallData& refballData = *ballData_;

            calc::vec3f& direction = refballData.direction;
            calc::vec3f& speed = refballData.speed;
            calc::mat4f& translation = refballData.translation;

            float x = (translation[0][3] += speed[0] * direction[0]);
            float y = (translation[1][3] += speed[1] * direction[1]);

            float m = n / 2.0 - 2.0;
            if (std::abs(x - m) < 0.5 || std::abs(x + m) < 0.5) {
                direction[0] *= -1;
            }

            if (std::abs(y - m) < 0.5 || std::abs(y + m) < 0.5) {
                direction[1] *= -1;
            }

            mats.push_back(translation * calc::rotate_4x(calc::radians(SDL_GetTicks() / 100)));

            std::vector<float> values(mats.size() * 16);
            for (::size_t i = 0; i != mats.size(); ++i)
            {
                mats[i] = calc::transpose(mats[i]);
                ::memcpy(&values[i * 16], calc::data(mats[i]), sizeof(calc::mat4f));
            }

            render::box& refshape = *refballData.shape;
            refshape.reset(values.data(), values.size() / 16);
            refshape.draw();

            panel_.render(refballData, refcamera);
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

    const unsigned shapeTAO[] = {
        render::load_texture_from_file("../images/shocked-face.png"),
        render::load_texture_from_file("../images/brick-wall.png")
    };

    ::size_t shapeTAOSize = sizeof(shapeTAO) / sizeof(unsigned);
    std::shared_ptr<BallData> data = std::make_shared<BallData>(new render::box(shapeTAO, shapeTAOSize, 100));

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
    static float zPos = -14.0;
    static float fov  = 30.0;
    static float zFar = 1000.0;

    Runner runner(params.window, new Camera(calc::vec3f(xPos, yPos, zPos), fov, zFar));
    runner.set_data(data);
    runner.run(params);

    SDL_StopTextInput();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    return 0;
}
