#pragma once

#ifndef CTRL_PANEL_HPP
#define CTRL_PANEL_HPP

// Fwd. decl.
struct BallData;

struct CtrlPanel {

    SDL_Window* window;

    float pitch;
    float yaw;
    float roll;

    bool enableGrid;

    bool run;
    bool firstCall;

    float gridColor[3];
    float backgroundColor[3];

    /*! ctor.
     */
    explicit CtrlPanel(SDL_Window* window);

    /*! Renders entire panel
     */
    void render(BallData& refballData, Camera& refcamera, unsigned* skinHandles, unsigned skinHandlesCount);

    void stop(BallData& refballData) const;
    void reset(BallData& refballData) const;

    // Helper
    void render_ball_subpanel(BallData& refballData, unsigned* textures, unsigned  textureCount);
    // Helper
    void render_background_subpanel();
    // Helper
    void render_scene_subpanel(Camera& refcamera);
    // Helper
    bool render_scene_angle_subpanel(Camera& refcamera);
    // Helper
    bool render_scene_position_subpanel(Camera& refcamera);
};

#endif
