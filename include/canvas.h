
// Function declarations
#ifndef CANVAS_H
#define CANVAS_H

typedef struct {
    int width;
    int height;
    float* pixels;   // flat array: y * width + x
    float* zbuffer;  // flat array: y * width + x
} canvas_t;

canvas_t* create_canvas(int width, int height);
void destroy_canvas(canvas_t* canvas);
void clear_canvas(canvas_t* canvas, float value);
void set_pixel_f(canvas_t* canvas, float x, float y, float intensity);
void draw_line_f(canvas_t* canvas, float x0, float y0, float x1, float y1, float intensity, float z0, float z1);
void save_canvas_ppm(canvas_t* canvas, const char* filename);
void draw_line_2d(canvas_t* canvas, float x0, float y0, float x1, float y1, float intensity);
void save_canvas_png(canvas_t* canvas, const char* filename);

#endif