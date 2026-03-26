// canvas.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


#include "canvas.h"

#define PI 3.14159265

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

canvas_t* create_canvas(int width, int height) {
    canvas_t* c = malloc(sizeof(canvas_t));
    c->width = width;
    c->height = height;
    c->pixels = malloc(width * height * sizeof(float));
    c->zbuffer = malloc(width * height * sizeof(float));
    clear_canvas(c, 0.0f);
    return c;
}

void destroy_canvas(canvas_t* canvas) {
    free(canvas->pixels);
    free(canvas->zbuffer);
    free(canvas);
}

void clear_canvas(canvas_t* canvas, float value) {
    for (int i = 0; i < canvas->width * canvas->height; i++) {
        canvas->pixels[i] = value;
        canvas->zbuffer[i] = 1.0f;  // farthest depth
    }
}

void set_pixel_f(canvas_t* canvas, float x, float y, float intensity) {
    int x0 = (int)floorf(x);
    int y0 = (int)floorf(y);
    float dx = x - x0;
    float dy = y - y0;

    for (int iy = 0; iy <= 1; iy++) {
        for (int ix = 0; ix <= 1; ix++) {
            int xi = x0 + ix;
            int yi = y0 + iy;
            if (xi >= 0 && xi < canvas->width && yi >= 0 && yi < canvas->height) {
                float weight = (1 - fabsf(ix - dx)) * (1 - fabsf(iy - dy));
                int index = yi * canvas->width + xi;
                canvas->pixels[index] += intensity * weight;
                if (canvas->pixels[index] > 1.0f) canvas->pixels[index] = 1.0f;
            }
        }
    }
}

void draw_line_2d(canvas_t* canvas, float x0, float y0, float x1, float y1, float intensity) {
    float dx = x1 - x0;
    float dy = y1 - y0;
    float length = sqrtf(dx * dx + dy * dy);
    int steps = (int)(length * 2.0f);  // increase for smoother lines if needed

    for (int i = 0; i <= steps; i++) {
        float t = (float)i / steps;
        float x = x0 + t * dx;
        float y = y0 + t * dy;
        set_pixel_f(canvas, x, y, intensity);  // uses bilinear filtering internally
    }
}

void draw_line_f(canvas_t* canvas, float x0, float y0, float x1, float y1, float intensity, float z0, float z1) {
    float dx = x1 - x0;
    float dy = y1 - y0;
    float dz = z1 - z0;
    float length = sqrtf(dx*dx + dy*dy);
    int steps = (int)(length * 2.0f);

    for (int i = 0; i <= steps; i++) {
        float t = (float)i / steps;
        float x = x0 + t * dx;
        float y = y0 + t * dy;
        float z = z0 + t * dz;

        for (float ty = -0.5f; ty <= 0.5f; ty += 0.5f) {
            for (float tx = -0.5f; tx <= 0.5f; tx += 0.5f) {
                int xi = (int)(x + tx);
                int yi = (int)(y + ty);
                if (xi >= 0 && xi < canvas->width && yi >= 0 && yi < canvas->height) {
                    int index = yi * canvas->width + xi;
                    if (z < canvas->zbuffer[index]) {
                        canvas->zbuffer[index] = z;
                        canvas->pixels[index] = intensity;
                    }
                }
            }
        }
    }
}

void save_canvas_ppm(canvas_t* canvas, const char* filename) {
    FILE* f = fopen(filename, "w");
    if (!f) {
        perror("Failed to open file for PPM");
        return;
    }

    fprintf(f, "P3\n%d %d\n255\n", canvas->width, canvas->height);

    for (int y = 0; y < canvas->height; y++) {
        for (int x = 0; x < canvas->width; x++) {
            int index = y * canvas->width + x;
            float intensity = canvas->pixels[index];
            int color = (int)(intensity * 255.0f);
            if (color < 0) color = 0;
            if (color > 255) color = 255;
            fprintf(f, "%d %d %d ", color, color, color);
        }
        fprintf(f, "\n");
    }

    fclose(f);
}

void save_canvas_png(canvas_t* canvas, const char* filename) {
    int width = canvas->width;
    int height = canvas->height;
    unsigned char* image_data = malloc(width * height * 3); // RGB format

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int index = y * width + x;
            float intensity = canvas->pixels[index];
            if (intensity < 0.0f) intensity = 0.0f;
            if (intensity > 1.0f) intensity = 1.0f;
            unsigned char gray = (unsigned char)(intensity * 255.0f);

            int rgb_index = index * 3;
            image_data[rgb_index + 0] = gray;
            image_data[rgb_index + 1] = gray;
            image_data[rgb_index + 2] = gray;
        }
    }

    stbi_write_png(filename, width, height, 3, image_data, width * 3);
    free(image_data);
}
