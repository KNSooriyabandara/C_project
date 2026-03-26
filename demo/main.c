#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <direct.h>

#include "canvas.h"
#include "math3d.h"
#include "renderer.h"
#include "animation.h"

#define PI 3.14159265f

int main() {
    _mkdir("clockface");
    _mkdir("fully_rendered_scene");

    int width = 1000;
    int height = 1000;
    float cx = width / 2.0f;
    float cy = height / 2.0f;
    float radius = 500.0f;

    // === Draw clock face once ===
    canvas_t* clock_canvas = create_canvas(width, height);
    clear_canvas(clock_canvas, 0.0f);
    for (int angle = 0; angle < 360; angle += 15) {
        float rad = angle * PI / 180.0f;
        float x = cx + radius * cosf(rad);
        float y = cy + radius * sinf(rad);
        draw_line_2d(clock_canvas, cx, cy, x, y, 1.0f);
    }
    save_canvas_png(clock_canvas, "clockface/clock.png");
    destroy_canvas(clock_canvas);
    printf("Saved clock face to clock.png\n");

    // === Global Settings ===
    int total_frames = 90;

    // === Initial ball center (fixed) ===
    vec3_t ball_center = { 0.0f, 1.5f, 0.0f };

    // === Bézier camera control points relative to ball_center ===
    vec3_t p0_rel = { 10, 3, 0 };
    vec3_t p1_rel = { 0, 6, 10 };
    vec3_t p2_rel = { -10, 0, 0 };
    vec3_t p3_rel = { 10, 3, 0 };

    // === Projection Setup ===
    float near = 1.5f;
    float far = 100.0f;
    float fov = 60.0f * (PI / 180.0f);
    float aspect = (float)width / (float)height;
    float top = near * tanf(fov / 2.0f);
    float bottom = -top;
    float right = top * aspect;
    float left = -right;
    mat4_t proj = mat4_frustum_asymmetric(left, right, bottom, top, near, far);

    // === Bouncing Ball State ===
    float ball_y = 1.5f;
    float ball_velocity = 20.0f;
    const float gravity = -100.0f;
    const float bounce_damping = 0.8f;

    // === Frame Loop ===
    for (int frame = 0; frame < total_frames; ++frame) {
        char filename[64];
        sprintf(filename, "fully_rendered_scene/scene_%03d.png", frame);

        canvas_t* canvas = create_canvas(width, height);
        clear_canvas(canvas, 0.0f);

        mesh_t cube, soccerball;
        generate_cube(&cube);
        generate_soccer_ball(&soccerball);

        // === Satellite-like Camera on Bézier orbit relative to fixed ball center ===
        float t = (float)frame / (total_frames - 1);
        vec3_t cam_pos_rel = bezier(p0_rel, p1_rel, p2_rel, p3_rel, t);
        vec3_t cam_pos = {
                            cam_pos_rel.x + ball_center.x,
                             cam_pos_rel.y + ball_center.y,
                            cam_pos_rel.z + ball_center.z
                        };

        vec3_t target = ball_center;   // Always look at the initial ball center
        vec3_t up = (vec3_t){ 0, 1, 0 };
        mat4_t view = look_at_matrix(cam_pos, target, up);

        // === Light sources ===
        vec3_t light_pos1 = { 0, 0, -5 };
        vec3_t light_pos2 = { -5, 0, 0 };
        vec3_t light_pos3 = { -3, -5, -3 };
        vec3_t light_pos4 = { -5, -5, 0 };
        vec3_t light_pos5 = { 0, -5, 0 };

        vec3_t light_dirs[5];
        vec3_t scene_target = { 0, 0, 0 };
        light_dirs[0] = vec3_normalize(vec3_sub(scene_target, light_pos1));
        light_dirs[1] = vec3_normalize(vec3_sub(scene_target, light_pos2));
        light_dirs[2] = vec3_normalize(vec3_sub(scene_target, light_pos3));
        light_dirs[3] = vec3_normalize(vec3_sub(scene_target, light_pos4));
        light_dirs[4] = vec3_normalize(vec3_sub(scene_target, light_pos5));


        // === Render Rotating Cube ===
        mat4_t cube_scale = mat4_scale(0.6f, 0.6f, 0.6f);
        float cube_angle = -((float)frame / total_frames) * 2.0f * PI;
        mat4_t cube_rotation = mat4_rotate_y(cube_angle);
        mat4_t cube_model = mat4_mul(&cube_rotation, &cube_scale);
        mat4_t cube_mvp = mat4_mul(&proj, &view);
        cube_mvp = mat4_mul(&cube_mvp, &cube_model);
        render_edges_from_faces_with_lighting_multi(canvas, &cube, &cube_mvp, &cube_model, light_dirs, 5);

        // === Bouncing Ball Physics ===
        float dt = 1.0f / 60.0f;
        ball_velocity += gravity * dt;
        ball_y += ball_velocity * dt;
        if (ball_y < 1.5f) {
            ball_y = 1.5f;
            ball_velocity = -ball_velocity * bounce_damping;
        }

        // === Render Soccer Ball ===
        float ball_angle = ((float)frame / total_frames) * 2.0f * PI;
        mat4_t ball_rotation = mat4_rotate_y(ball_angle);
        mat4_t ball_scale = mat4_scale(0.4f, 0.4f, 0.4f);
        mat4_t ball_translate = mat4_translate(0, ball_y, 0);
        mat4_t ball_model = mat4_mul(&ball_translate, &ball_rotation);
        ball_model = mat4_mul(&ball_model, &ball_scale);
        mat4_t ball_mvp = mat4_mul(&proj, &view);
        ball_mvp = mat4_mul(&ball_mvp, &ball_model);
        render_edges_from_faces_with_lighting_multi(canvas, &soccerball, &ball_mvp, &ball_model, light_dirs, 2);

        // === Save frame ===
        save_canvas_png(canvas, filename);
        printf("Frame %d | ball_y: %.2f vel: %.2f | cam_pos: (%.2f, %.2f, %.2f) -> %s\n",
               frame, ball_y, ball_velocity, cam_pos.x, cam_pos.y, cam_pos.z, filename);

        destroy_canvas(canvas);
        free(cube.vertices); free(cube.faces); free(cube.edges);
        free(soccerball.vertices); free(soccerball.faces);
    }

    return 0;
}
