#include <memory>
#include <vector>

#include <glad/glad.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include "dear_imgui/imgui.h"
#include "dear_imgui_backends/imgui_impl_opengl3.h"
#include "dear_imgui_backends/imgui_impl_sdl.h"

#include "images/tiles/dark_grass.h"
#include "images/tiles/dry_grass.h"

#include "images/awesome_face.h"
#include "images/brick_wall.h"
#include "images/incredulous_face.h"
#include "images/shocked_face.h"

#include "ball_data.hpp"
#include "box.hpp"
#include "camera.hpp"
#include "ctrl_panel.hpp"
#include "draw_instanced_no_texture.hpp"
#include "draw_instanced_with_texture.hpp"
#include "grid_square.hpp"
#include "square.hpp"
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

namespace{

    /*! Helper
     *! Builds the vertices for the map grid
     */
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

    /*! Helper
     *! Build the vertices for the map wall
     */
    std::vector<calc::mat4f> build_wall(int width, int length)
    {
        std::vector<calc::mat4f> wall;

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

    /*! Class Runner
     *! Encapsulates the main loop
     */
    class Runner {
    public:

        /*! ctor.
         */
        Runner(SDL_Window* window, Camera* camera) : window_(window)
                                                   , panel_(window)
                                                   , camera_(camera) {
            static const unsigned width = 30;
            static const unsigned height = 30;

            cageWidth_ = width + (width % 2);
            cageLength_ = height + (height % 2);

            gridWidth_ = 2 * cageWidth_;
            gridLength_ = 2 * cageLength_;

            grid_ = copy_matrix_data(build_grid(gridWidth_, gridLength_));
            wall_ = copy_matrix_data(build_wall(cageWidth_, cageLength_));

            unsigned boxTAO1[] = {
                render::load_texture_from_data(brick_wall_png, brick_wall_png_len, false),
                render::load_texture_from_data(awesome_face_png, awesome_face_png_len, true)
            };

            unsigned boxTAO2[] = {
                boxTAO1[0],
                render::load_texture_from_data(shocked_face_png, shocked_face_png_len, true)
            };

            unsigned boxTAO3[] = {
                boxTAO1[0],
                render::load_texture_from_data(incredulous_face_png, incredulous_face_png_len, true)
            };

            unsigned wallTAO[] = {
                boxTAO1[0],
                boxTAO1[0],
            };

            textureHandles_.push_back(render::load_texture_from_data(awesome_face_png,
                                                                     awesome_face_png_len,
                                                                     true,
                                                                     false));
            textureHandles_.push_back(render::load_texture_from_data(shocked_face_png,
                                                                     shocked_face_png_len,
                                                                     true,
                                                                     false));
            textureHandles_.push_back(render::load_texture_from_data(incredulous_face_png,
                                                                     incredulous_face_png_len,
                                                                     true,
                                                                     false));

            ballObject_[0] = render::Box(boxTAO1, (sizeof(boxTAO1) / sizeof(unsigned)), 1);
            ballObject_[0].push_back(calc::mat4f::identity());

            ballObject_[1] = render::Box(boxTAO2, (sizeof(boxTAO2) / sizeof(unsigned)), 1);
            ballObject_[1].push_back(calc::mat4f::identity());

            ballObject_[2] = render::Box(boxTAO3, (sizeof(boxTAO3) / sizeof(unsigned)), 1);
            ballObject_[2].push_back(calc::mat4f::identity());

            wallObject_ = render::Box(wallTAO, (sizeof(wallTAO) / sizeof(unsigned)), (cageWidth_ * cageLength_));
            gridTile_ = render::GridSquare((gridWidth_ * gridLength_));

            // Load dry grass tiles
            unsigned dryGrassTextureTAO = render::load_texture_from_data(dry_grass_png, dry_grass_png_len, false);
            unsigned dryGrassTileTAO[] = {
                dryGrassTextureTAO,
                dryGrassTextureTAO
            };

            dryGrassTile_ = render::Square(dryGrassTileTAO, sizeof(dryGrassTileTAO) / sizeof(unsigned),
                                           std::pow(gridWidth_ * gridLength_, 2));

            int gridHalfLength = gridLength_ / 2;
            int gridHalfWidth = gridWidth_ / 2;

            int cageHalfLength = cageLength_ / 2;
            int cageHalfWidth = cageWidth_ / 2;

            calc::mat4f mat = calc::mat4f::identity();

            for (int i = -gridHalfLength; i != 1 - cageHalfLength; ++i)
            {
                for (int j = -gridHalfWidth; j != gridHalfWidth + 1; ++j)
                {
                    mat[3][0] = j;
                    mat[3][1] = i;
                    dryGrassTile_.push_back(mat);
                }
            }

            for (int i = 1 - cageHalfLength; i != cageHalfLength; ++i)
            {
                for (int j = -gridHalfWidth; j != -cageHalfWidth + 1; ++j)
                {
                    mat[3][0] = j;
                    mat[3][1] = i;
                    dryGrassTile_.push_back(mat);
                }
            }

            for (int i = 1 - cageHalfLength; i != cageHalfLength; ++i)
            {
                for (int j = cageHalfWidth; j != gridHalfWidth + 1; ++j)
                {
                    mat[3][0] = j;
                    mat[3][1] = i;
                    dryGrassTile_.push_back(mat);
                }
            }

            for (int i = cageHalfLength; i != gridHalfLength + 1; ++i)
            {
                for (int j = -gridHalfWidth; j != gridHalfWidth + 1; ++j)
                {
                    mat[3][0] = j;
                    mat[3][1] = i;
                    dryGrassTile_.push_back(mat);
                }
            }

            // Load grass tiles
            unsigned grassTextureTAO = render::load_texture_from_data(dark_grass_png, dark_grass_png_len, false);
            unsigned grassTileTAO[] = {
                grassTextureTAO,
                grassTextureTAO
            };

            grassTile_ = render::Square(grassTileTAO, sizeof(grassTileTAO) / sizeof(unsigned), cageWidth_ * cageLength_);

            for (int i = 1 - cageHalfLength; i != cageHalfLength; ++i)
            {
                for (int j = 1 - cageHalfWidth; j != cageHalfWidth; ++j)
                {
                    grassTile_.push_back(calc::transpose([i, j]() {

                        calc::mat4f mat = calc::mat4f::identity();
                        mat[0][3] = j;
                        mat[1][3] = i;
                        mat[2][3] = 0;
                        return mat;
                    }()));
                }
            }
        }

        /*! Run loop
         */
        void run() {

            while ((panel_.run))
            {
                //Handle events in queue
                SDL_Event e;
                while (SDL_PollEvent(&e) != 0)
                {
                    if (e.type == SDL_QUIT) {
                        return;
                    }

                    ImGui_ImplSDL2_ProcessEvent(&e);
                    switch (e.type)
                    {
                        // Handle window-related events
                        case SDL_WINDOWEVENT:
                        {
                            on_window_event(e);
                            break;
                        }

                        // Handle keypress...
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

                // Render the scene
                render();
            }
        }

    private:

        /*! Helper
         *! Evt. handler
         */
        void on_window_event(const SDL_Event& e) {

            if ((e.window).event == SDL_WINDOWEVENT_SIZE_CHANGED)
            {
                unsigned screenWidth = e.window.data1;
                unsigned screenHeight = e.window.data2;
                glViewport(0, 0, screenWidth, screenHeight);

                // Update camera
                Camera& refcamera = *camera_;
                refcamera.resize(screenWidth, screenHeight);
                refcamera.update();
            }
        }

        /*! Helper
         *! Evt. handler
         */
        void on_text_input(const SDL_Event&) {

            if ((ImGui::GetIO()).WantCaptureKeyboard) {
                return;
            }
        }

        /*! Helper
         *! Renders the scene
         */
        void render() {

            glClearColor(panel_.backgroundColor[0],
                         panel_.backgroundColor[1],
                         panel_.backgroundColor[2],
                         1.0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            const calc::mat4f& lookAt     = camera_->get_device_look_at();
            const calc::mat4f& projection = camera_->get_device_projection();

            // Maybe draw the grid
            if (panel_.enableGrid)
            {
                gridDraw_.use();
                gridDraw_.set_color(calc::vec4f(panel_.gridColor[0],
                                                panel_.gridColor[1],
                                                panel_.gridColor[2],
                                                1.0));
                gridDraw_.set_scene(lookAt, projection);
                gridTile_.reset(grid_.data(), grid_.size() / 16);
                gridTile_.draw();
            }

            // Draw the wall
            mainDraw_.use();
            mainDraw_.set_scene(lookAt, projection);
            wallObject_.reset(wall_.data(), wall_.size() / 16);
            wallObject_.draw();

            // Draw the grass outside the cage
            dryGrassTile_.draw();
            // Draw the grass inside the cage
            grassTile_.draw();

            // Draw the box
            calc::vec3f& direction = ballData_.direction;
            calc::vec3f& speed = ballData_.speed;
            calc::mat4f& translation = ballData_.translation;

            translation[2][3] = -1.0;
            float x = (translation[0][3] += speed[0] * direction[0]);
            float y = (translation[1][3] += speed[1] * direction[1]);

            // Bounce back on wall hit
            float hitOffset = 3.0;
            if (x < +hitOffset - (cageWidth_ / 2) ||
                x > -hitOffset + (cageWidth_ / 2)) {
                direction[0] *= -1;
            }

            if (y < +hitOffset - (cageLength_ / 2) ||
                y > -hitOffset + (cageLength_ / 2)) {
                direction[1] *= -1;
            }

            const calc::vec3f turnRate = ballData_.turnRate * calc::radians(SDL_GetTicks() / 10.0);
            const calc::mat4f boxMat = calc::transpose(translation
                                                       * calc::rotate_4x(turnRate[0])
                                                       * calc::rotate_4y(turnRate[1])
                                                       * calc::rotate_4z(turnRate[2]));

            render::Box& refobject = ballObject_[ballData_.selectedSkin];
            refobject.modify(calc::data(boxMat), 0);
            refobject.draw();

            // Draw the control panel
            panel_.render(ballData_, *camera_, textureHandles_.data(), textureHandles_.size());
            // Update screen & return
            SDL_GL_SwapWindow(window_);
        }

        // Points to main SDL window
        SDL_Window* window_;

        // Control panel
        CtrlPanel panel_;

        // Camera / viewer
        Camera* camera_;
        // Contains ball position and rotation information
        BallData ballData_;

        // Program, uses instancing;
        // called to draw grid squares
        DrawInstancedNoTexture gridDraw_;
        // Program, uses instancing;
        // called to draw all textured objects
        DrawInstancedWithTexture mainDraw_;

        // Map item
        render::Square     grassTile_;
        // Map item
        render::Square     dryGrassTile_;
        // Map item
        render::GridSquare gridTile_;
        // Map item
        render::Box        ballObject_[3];
        // Map item
        render::Box        wallObject_;

        std::vector<unsigned> textureHandles_;

        // Dimension
        unsigned gridWidth_;
        // Dimension
        unsigned gridLength_;

        // Dimension
        unsigned cageWidth_;
        // Dimension
        unsigned cageLength_;

        // Wall vertices
        std::vector<float> wall_;
        // Grid Vertices
        std::vector<float> grid_;
    };
}

/*! Entry point
 */
int main(void)
{
    unsigned screenWidth = 800;
    unsigned screenHeight = 800;

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
    ImGui_ImplOpenGL3_Init();

    // Init camera defaults
    static float xPos = 0;
    static float yPos = 0;
    static float zPos = -20.0;
    static float fov  = 30.0;
    static float zFar = 1000.0;

    // Init camera
    std::shared_ptr<Camera> camera(new Camera(calc::vec3f(xPos, yPos, zPos), fov, zFar));
    camera->set_scene_rotation(25, 0, 0);

    // Enter run loop
    Runner runner(params.window, camera.get());
    runner.run();

    SDL_StopTextInput();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    return 0;
}
