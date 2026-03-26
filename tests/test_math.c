#include <stdio.h>
#include "math3d.h"

int main() {
    // Step 1: Define a unit cube centered at origin
    vec3_t cube_vertices[8] = {
        {-0.5, -0.5, -0.5}, {0.5, -0.5, -0.5},
        {0.5,  0.5, -0.5}, {-0.5,  0.5, -0.5},
        {-0.5, -0.5,  0.5}, {0.5, -0.5,  0.5},
        {0.5,  0.5,  0.5}, {-0.5,  0.5,  0.5}
    };

    printf("Original cube vertices:\n");
    for (int i = 0; i < 8; i++) {
        printf("Vertex %d: (%.2f, %.2f, %.2f)\n", i, cube_vertices[i].x, cube_vertices[i].y, cube_vertices[i].z);
    }

    // Step 2: Create world transform (rotate + translate)
    float rx = 0.5f, ry = 0.5f, rz = 0.0f;  // rotation angles in radians
    mat4_t rot = mat4_rotate_xyz(rx, ry, rz); // rotate around X then Y
    mat4_t trans = mat4_translate(0.0f, 0.0f, -3.0f); // move cube into view
    mat4_t model = mat4_mul(&trans, &rot); // model = T * R

    printf("\nAfter applying rotation (X=%.2f rad, Y=%.2f rad):\n", rx, ry);
    for (int i = 0; i < 8; i++) {
        vec3_t transformed = mat4_mul_vec3(&model, cube_vertices[i]);
        printf("Vertex %d: (%.2f, %.2f, %.2f)\n", i, transformed.x, transformed.y, transformed.z);
    }

    // Step 3: Create projection matrix
    mat4_t proj = mat4_frustum_asymmetric(-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 10.0f);

    // Step 4: Combine into MVP matrix
    mat4_t mvp = mat4_mul(&proj, &model); // MVP = P * M

    printf("\nProjected cube vertices:\n");
    for (int i = 0; i < 8; i++) {
        vec3_t v = cube_vertices[i];
        vec3_t projected = mat4_mul_vec3(&mvp, v);

        // Perspective divide approximation
        float w = -v.z - 3.0f;
        if (w != 0.0f) {
            projected.x /= w;
            projected.y /= w;
            projected.z /= w;
        }

        printf("Vertex %d: (%.2f, %.2f, %.2f)\n", i, projected.x, projected.y, projected.z);
    }

    return 0;
}
