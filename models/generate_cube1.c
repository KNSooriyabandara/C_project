#include <stdlib.h>
#include "math3d.h"
#include "renderer.h"

void generate_cube1(mesh_t* mesh) {
    mesh->vertex_count = 8;
    mesh->vertices = malloc(sizeof(vec3_t) * mesh->vertex_count);

    float s = 1.0f;
    vec3_t cube_vertices[] = {
        {-s, -s, -s}, { s, -s, -s},
        { s,  s, -s}, {-s,  s, -s},
        {-s, -s,  s}, { s, -s,  s},
        { s,  s,  s}, {-s,  s,  s}
    };
    for (int i = 0; i < 8; ++i)
        mesh->vertices[i] = cube_vertices[i];

    // Triangulated faces (12 triangles for 6 square faces)
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

    // Only actual cube edges (no diagonals)
    mesh->edge_count = 12;
    mesh->edges = malloc(sizeof(int[2]) * mesh->edge_count);
    int edges[][2] = {
        {0, 1}, {1, 2}, {2, 3}, {3, 0}, // bottom
        {4, 5}, {5, 6}, {6, 7}, {7, 4}, // top
        {0, 4}, {1, 5}, {2, 6}, {3, 7}  // sides
    };
    for (int i = 0; i < 12; ++i)
        for (int j = 0; j < 2; ++j)
            mesh->edges[i][j] = edges[i][j];
}
