#include <stdlib.h>
#include "math3d.h"
#include "renderer.h"

void generate_cube(mesh_t* mesh) {
    mesh->vertex_count = 8;
    mesh->vertices = malloc(sizeof(vec3_t) * mesh->vertex_count);

    float s = 1.0f;
    vec3_t cube_vertices[] = {
        { -s, -s, -s }, {  s, -s, -s },
        {  s,  s, -s }, { -s,  s, -s },
        { -s, -s,  s }, {  s, -s,  s },
        {  s,  s,  s }, { -s,  s,  s }
    };
    for (int i = 0; i < 8; ++i)
        mesh->vertices[i] = cube_vertices[i];

    // === Triangle faces (same as before) ===
    mesh->face_count = 12;
    mesh->faces = malloc(sizeof(int[3]) * mesh->face_count);
    int faces[][3] = {
        {0, 2, 1}, {0, 3, 2}, // bottom
        {4, 5, 6}, {4, 6, 7}, // top
        {0, 5, 4}, {0, 1, 5}, // front
        {1, 6, 5}, {1, 2, 6}, // right
        {2, 7, 6}, {2, 3, 7}, // back
        {3, 4, 7}, {3, 0, 4}  // left
    };
    for (int i = 0; i < 12; ++i)
        for (int j = 0; j < 3; ++j)
            mesh->faces[i][j] = faces[i][j];

    // === Unique edges per face (3 edges × 12 faces = 36 edges) ===
    mesh->edge_count = 36;
    mesh->edges = malloc(sizeof(int[2]) * mesh->edge_count);
    int edge_index = 0;
    for (int i = 0; i < 12; ++i) {
        int v0 = faces[i][0];
        int v1 = faces[i][1];
        int v2 = faces[i][2];
        mesh->edges[edge_index][0] = v0;
        mesh->edges[edge_index][1] = v1;
        edge_index++;
        mesh->edges[edge_index][0] = v1;
        mesh->edges[edge_index][1] = v2;
        edge_index++;
        mesh->edges[edge_index][0] = v2;
        mesh->edges[edge_index][1] = v0;
        edge_index++;
    }
}
