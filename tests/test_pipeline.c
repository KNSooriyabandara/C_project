
#include <direct.h>
#include <stdio.h>
#include <stdlib.h>
#include "canvas.h"
#include "math3d.h"
#include "renderer.h"
#include <math.h>
#define PI 3.14159265f





// Declare mesh generators
void generate_cube(mesh_t* mesh);
void generate_soccer_ball(mesh_t* mesh);

int main() {
    _mkdir("visuals");
    _mkdir("visuals/test_cube");
    _mkdir("visuals/test_soccer");
    int width = 1080, height = 1080;

    // === STATIC CUBE RENDERING ===
    {
        canvas_t* canvas = create_canvas(width, height);
        clear_canvas(canvas, 0.0f);

        mesh_t cube;
        generate_cube1(&cube);  // cube.vertices + cube.edges must be filled

        mat4_t model = mat4_rotate_xyz(0.3f, 0.6f, 0.2f);
        mat4_t view = mat4_translate(0.0f, 0.0f, -5.0f);

        float near = 1.0f, far = 10.0f;
        float left = -1.0f, right = 1.0f;
        float bottom = -1.0f, top = 1.0f;
        mat4_t proj = mat4_frustum_asymmetric(left, right, bottom, top, near, far);

        mat4_t mvp = mat4_mul(&proj, &view);
        mvp = mat4_mul(&mvp, &model);

        render_edges_only(canvas, &cube, &mvp);
        save_canvas_png(canvas, "visuals/test_cube/cube_output.png");
        printf("Rendered cube to cube_output.png\n");

        free(cube.vertices);
        free(cube.edges);
        destroy_canvas(canvas);
    }

    // === SOCCER BALL ANIMATION ===
    {
        int total_frames = 90;

        vec3_t cam_pos = { 0, 0, -6 };
        vec3_t target = { 0, 0, 0 };
        vec3_t up = { 0, 1, 0 };

        float near = 0.1f, far = 100.0f;
        float fov = 60.0f * (PI / 180.0f);
        float aspect = (float)width / (float)height;
        float top = near * tanf(fov / 2.0f);
        float bottom = -top;
        float right = top * aspect;
        float left = -right;

        mat4_t proj = mat4_frustum_asymmetric(left, right, bottom, top, near, far);
        mat4_t view = look_at_matrix(cam_pos, target, up);

        for (int frame = 0; frame < total_frames; ++frame) {
            char filename[64];
            sprintf(filename, "visuals/test_soccer/soccerball_%03d.png", frame);

            canvas_t* canvas = create_canvas(width, height);
            clear_canvas(canvas, 0.0f);

            mesh_t soccerball;
            generate_soccer_ball(&soccerball);  // mesh must have vertices and edges

            float angle = ((float)frame / total_frames) * 2.0f * PI;
            mat4_t rotation = mat4_rotate_y(angle);
            mat4_t scale = mat4_scale(0.4f, 0.4f, 0.4f);
            mat4_t model = mat4_mul(&rotation, &scale);

            mat4_t mvp = mat4_mul(&proj, &view);
            mvp = mat4_mul(&mvp, &model);

            render_edges_only(canvas, &soccerball, &mvp);
            save_canvas_png(canvas, filename);
            printf("Saved frame: %s\n", filename);

            destroy_canvas(canvas);
            free(soccerball.vertices);
            free(soccerball.edges);  // if generated
            free(soccerball.faces);
        }
    }

    return 0;
}
