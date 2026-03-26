#include "renderer.h"
#include <math.h>
#include <stdlib.h>
#include "lighting.h"
#include <stdbool.h>

// === Constants ===
#define LOG_DEPTH_SCALE 1.0f  // Tune based on your near plane
#define MAX_VERTICES 60       // Adjust as needed for your mesh max vertices

// === Data structures for edge tracking ===
static bool original_edge[MAX_VERTICES][MAX_VERTICES] = {false};
static bool drawn_edge[MAX_VERTICES][MAX_VERTICES] = {false};

// === Helper: Transform a normal vector by model matrix (3x3 rotation part only) ===


// === Vertex Projection ===
vec3_t project_vertex(const vec3_t* v, const mat4_t* mvp, int screen_w, int screen_h) {
    float x = v->x, y = v->y, z = v->z;

    float px = mvp->m[0]*x + mvp->m[4]*y + mvp->m[8]*z + mvp->m[12];
    float py = mvp->m[1]*x + mvp->m[5]*y + mvp->m[9]*z + mvp->m[13];
    float pz = mvp->m[2]*x + mvp->m[6]*y + mvp->m[10]*z + mvp->m[14];
    float pw = mvp->m[3]*x + mvp->m[7]*y + mvp->m[11]*z + mvp->m[15];

    if (pw != 0.0f) {
        px /= pw;
        py /= pw;
        pz /= pw;
    }

    // Map depth from [-1,1] to [0,1] and apply logarithmic depth scaling
    float z_ndc = (pz + 1.0f) * 0.5f;
    float z_log = logf(z_ndc * LOG_DEPTH_SCALE + 1.0f);

    vec3_t projected;
    projected.x = (px + 1.0f) * 0.5f * screen_w;
    projected.y = (1.0f - py) * 0.5f * screen_h;
    projected.z = z_log;  // for fading intensity based on depth
    return projected;
}

// === Viewport Clipping to circular viewport ===
int clip_to_circular_viewport(canvas_t* canvas, float x, float y) {
    float cx = canvas->width / 2.0f;
    float cy = canvas->height / 2.0f;
    float radius = (canvas->width < canvas->height ? canvas->width : canvas->height) / 2.0f;
    float dx = x - cx;
    float dy = y - cy;
    return (dx*dx + dy*dy <= radius*radius);
}

// === Initialize original edges from mesh edges ===
void init_original_edges(const mesh_t* mesh) {
    for (int i = 0; i < mesh->edge_count; ++i) {
        int v0 = mesh->edges[i][0];
        int v1 = mesh->edges[i][1];
        original_edge[v0][v1] = true;
        original_edge[v1][v0] = true; // undirected edge
    }
}

// === Helper: Compute Lambert brightness summed over multiple lights ===


// === Render faces (triangles) with lighting from multiple lights ===
void render_faces_with_lighting_multi(canvas_t* canvas, const mesh_t* mesh, const mat4_t* mvp, const mat4_t* model, vec3_t* light_dirs, int light_count) {
    // Normalize all light directions once
    for (int i = 0; i < light_count; i++)
        light_dirs[i] = vec3_normalize_fast(light_dirs[i]);

    for (int i = 0; i < mesh->face_count; i++) {
        int i0 = mesh->faces[i][0];
        int i1 = mesh->faces[i][1];
        int i2 = mesh->faces[i][2];

        vec3_t v0 = mesh->vertices[i0];
        vec3_t v1 = mesh->vertices[i1];
        vec3_t v2 = mesh->vertices[i2];

        // Compute face normal in model space
        vec3_t u = vec3_sub(v1, v0);
        vec3_t v = vec3_sub(v2, v0);
        vec3_t normal_model = vec3_normalize_fast(vec3_cross(u, v));

        // Transform normal by model matrix to world space
        vec3_t normal_world = mat4_transform_normal(*model, normal_model);
        normal_world = vec3_normalize_fast(normal_world);

        // Compute brightness from multiple lights
        float intensity = lambert_brightness_multi(normal_world, light_dirs, light_count);

        // Project vertices
        vec3_t p0 = project_vertex(&v0, mvp, canvas->width, canvas->height);
        vec3_t p1 = project_vertex(&v1, mvp, canvas->width, canvas->height);
        vec3_t p2 = project_vertex(&v2, mvp, canvas->width, canvas->height);

        // Draw edges of triangle if inside viewport
        if (clip_to_circular_viewport(canvas, p0.x, p0.y) && clip_to_circular_viewport(canvas, p1.x, p1.y))
            draw_line_f(canvas, p0.x, p0.y, p1.x, p1.y, intensity, p0.z, p1.z);

        if (clip_to_circular_viewport(canvas, p1.x, p1.y) && clip_to_circular_viewport(canvas, p2.x, p2.y))
            draw_line_f(canvas, p1.x, p1.y, p2.x, p2.y, intensity, p1.z, p2.z);

        if (clip_to_circular_viewport(canvas, p2.x, p2.y) && clip_to_circular_viewport(canvas, p0.x, p0.y))
            draw_line_f(canvas, p2.x, p2.y, p0.x, p0.y, intensity, p2.z, p0.z);
    }
}

// === Render edges from faces with lighting from multiple lights (avoiding duplicate edges) ===
void render_edges_from_faces_with_lighting_multi(canvas_t* canvas, const mesh_t* mesh, const mat4_t* mvp, const mat4_t* model, vec3_t* light_dirs, int light_count) {
    // Normalize all light directions once
    for (int i = 0; i < light_count; i++)
        light_dirs[i] = vec3_normalize_fast(light_dirs[i]);

    // Initialize original edges once (assuming mesh edges are static)
    init_original_edges(mesh);

    for (int f = 0; f < mesh->face_count; ++f) {
        int i0 = mesh->faces[f][0];
        int i1 = mesh->faces[f][1];
        int i2 = mesh->faces[f][2];

        vec3_t v0 = mesh->vertices[i0];
        vec3_t v1 = mesh->vertices[i1];
        vec3_t v2 = mesh->vertices[i2];

        // Compute face normal in model space
        vec3_t u = vec3_sub(v1, v0);
        vec3_t v = vec3_sub(v2, v0);
        vec3_t normal_model = vec3_normalize_fast(vec3_cross(u, v));

        // Transform normal by model matrix to world space
        vec3_t normal_world = mat4_transform_normal(*model, normal_model);
        normal_world = vec3_normalize_fast(normal_world);

        // Compute brightness from multiple lights
        float intensity = lambert_brightness_multi(normal_world, light_dirs, light_count);

        // Triangle edges
        int edges[3][2] = {{i0, i1}, {i1, i2}, {i2, i0}};

        for (int e = 0; e < 3; e++) {
            int a = edges[e][0];
            int b = edges[e][1];

            // Draw only if this is an original edge
            if (original_edge[a][b]) {
                vec3_t p0 = project_vertex(&mesh->vertices[a], mvp, canvas->width, canvas->height);
                vec3_t p1 = project_vertex(&mesh->vertices[b], mvp, canvas->width, canvas->height);

                if (clip_to_circular_viewport(canvas, p0.x, p0.y) && clip_to_circular_viewport(canvas, p1.x, p1.y)) {
                    draw_line_f(canvas, p0.x, p0.y, p1.x, p1.y, intensity, p0.z, p1.z);
                }
            }
        }
    }
}


// === Optional: Render edges only without lighting, with depth fading ===
void render_edges_only(canvas_t* canvas, const mesh_t* mesh, const mat4_t* mvp) {
    for (int i = 0; i < mesh->edge_count; ++i) {
        int i0 = mesh->edges[i][0];
        int i1 = mesh->edges[i][1];

        vec3_t v0 = mesh->vertices[i0];
        vec3_t v1 = mesh->vertices[i1];

        vec3_t p0 = project_vertex(&v0, mvp, canvas->width, canvas->height);
        vec3_t p1 = project_vertex(&v1, mvp, canvas->width, canvas->height);

        if (clip_to_circular_viewport(canvas, p0.x, p0.y) &&
            clip_to_circular_viewport(canvas, p1.x, p1.y)) {

            // Log-depth fading
            float z_avg = (p0.z + p1.z) * 0.5f;
            float intensity = 1.0f - z_avg;
            intensity = powf(intensity, 1.5f);  // gamma curve
            if (intensity < 0.6f) intensity = 0.6f;
            if (intensity > 1.0f) intensity = 1.0f;

            draw_line_2d(canvas, p0.x, p0.y, p1.x, p1.y, intensity);
        }
    }
}
