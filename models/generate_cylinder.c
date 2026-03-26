#define _USE_MATH_DEFINES
#include <stdlib.h>
#include <math.h>
#include "renderer.h"

void generate_cylinder(mesh_t* mesh, float radius, float height, int segments) {
    int i;

    int vert_count = (segments + 1) * 2 + 2; // side vertices + 2 center vertices (top and bottom)
    int face_count = segments * 4; // 2 triangles per side + top + bottom

    mesh->vertex_count = vert_count;
    mesh->face_count = face_count;
    mesh->vertices = malloc(sizeof(vec3_t) * vert_count);
    mesh->faces = malloc(sizeof(int[3]) * face_count);
    mesh->edges = NULL;  // Not used for rendering

    float half_h = height / 2.0f;

    // Create side vertices (top and bottom circles)
    for (i = 0; i <= segments; i++) {
        float angle = 2 * M_PI * i / segments;
        float x = cosf(angle) * radius;
        float z = sinf(angle) * radius;
        mesh->vertices[i] = (vec3_t){x, half_h, z};               // Top circle
        mesh->vertices[i + segments + 1] = (vec3_t){x, -half_h, z}; // Bottom circle
    }

    // Add center top and bottom points
    int top_center_idx = 2 * (segments + 1);
    int bottom_center_idx = top_center_idx + 1;
    mesh->vertices[top_center_idx] = (vec3_t){0, half_h, 0};
    mesh->vertices[bottom_center_idx] = (vec3_t){0, -half_h, 0};

    // Side faces (2 triangles per quad)
    int f = 0;
    for (i = 0; i < segments; i++) {
        int top1 = i;
        int top2 = (i + 1) % (segments + 1);
        int bot1 = top1 + segments + 1;
        int bot2 = top2 + segments + 1;

        // First triangle of quad
        mesh->faces[f][0] = top1;
        mesh->faces[f][1] = bot1;
        mesh->faces[f][2] = top2;
        f++;

        // Second triangle of quad
        mesh->faces[f][0] = top2;
        mesh->faces[f][1] = bot1;
        mesh->faces[f][2] = bot2;
        f++;
    }

    // Top cap triangles
    for (i = 0; i < segments; i++) {
        mesh->faces[f][0] = top_center_idx;
        mesh->faces[f][1] = (i + 1) % (segments + 1);
        mesh->faces[f][2] = i;
        f++;
    }

    // Bottom cap triangles
    for (i = 0; i < segments; i++) {
        int b0 = i + segments + 1;
        int b1 = (i + 1) % (segments + 1) + segments + 1;
        mesh->faces[f][0] = bottom_center_idx;
        mesh->faces[f][1] = b0;
        mesh->faces[f][2] = b1;
        f++;
    }
}
