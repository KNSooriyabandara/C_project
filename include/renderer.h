#ifndef RENDERER_H
#define RENDERER_H

#include "canvas.h"
#include "math3d.h"
#include "lighting.h"

typedef struct {
    vec3_t* vertices;
    int vertex_count;
    int (*edges)[2];
    int edge_count;
    int (*faces)[3];     // NEW: triangle faces
    int face_count;      // NEW
} mesh_t;


vec3_t project_vertex(const vec3_t* v, const mat4_t* mvp, int screen_w, int screen_h);
int clip_to_circular_viewport(canvas_t* canvas, float x, float y);

void render_edges_from_faces_with_lighting_multi(canvas_t* canvas, const mesh_t* mesh, const mat4_t* mvp, const mat4_t* model, vec3_t* light_dirs, int light_count);
void render_faces_with_lighting_multi(canvas_t* canvas, const mesh_t* mesh, const mat4_t* mvp, const mat4_t* model, vec3_t* light_dirs, int light_count);
void render_edges_only(canvas_t* canvas, const mesh_t* mesh, const mat4_t* mvp);
void init_original_edges(const mesh_t* mesh);



#endif
