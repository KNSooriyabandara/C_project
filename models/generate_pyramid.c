#include <stdlib.h>
#include "math3d.h"
#include "renderer.h"

void generate_pyramid(mesh_t* mesh) {
    mesh->vertex_count = 5;
    mesh->vertices = malloc(sizeof(vec3_t) * mesh->vertex_count);

    float s = 1.0f;

    // 4 base corners
    mesh->vertices[0] = (vec3_t){ -s, 0, -s };
    mesh->vertices[1] = (vec3_t){  s, 0, -s };
    mesh->vertices[2] = (vec3_t){  s, 0,  s };
    mesh->vertices[3] = (vec3_t){ -s, 0,  s };

    // Apex
    mesh->vertices[4] = (vec3_t){ 0, s, 0 };

    // Optional: edges (for wireframe rendering)
    mesh->edge_count = 8;
    mesh->edges = malloc(sizeof(int[2]) * mesh->edge_count);
    int edges[][2] = {
        {0,1}, {1,2}, {2,3}, {3,0}, // base
        {0,4}, {1,4}, {2,4}, {3,4}  // sides
    };
    for (int i = 0; i < 8; ++i) {
        mesh->edges[i][0] = edges[i][0];
        mesh->edges[i][1] = edges[i][1];
    }

    // Triangle faces
    mesh->face_count = 6;
    mesh->faces = malloc(sizeof(int[3]) * mesh->face_count);
    int faces[][3] = {
        {0, 1, 2}, {0, 2, 3}, // base
        {0, 1, 4}, {1, 2, 4}, // sides
        {2, 3, 4}, {3, 0, 4}
    };
    for (int i = 0; i < 6; ++i)
        for (int j = 0; j < 3; ++j)
            mesh->faces[i][j] = faces[i][j];
}
