# libtiny3d - 3D Software Renderer in C

This project is a lightweight 3D software renderer implemented in C. It supports 3D transformations, perspective projection, simple lighting, and wireframe rendering. This README provides an overview of the software, its file structure, usage, and public APIs exposed through header files.

---

## 📁 Table of Contents

1. [Folder Structure](#folder-structure)
2. [Functionality and Header Files](#functionality-and-header-files)
3. [Building and Usage](#building-and-usage)
4. [License](#license)

---

## 📁 Folder Structure

```
libtiny3d/
├── include/
│   ├── tiny3d.h
│   ├── canvas.h
│   ├── math3d.h
│   ├── renderer.h
│   └── lighting.h
├── src/
│   ├── canvas.c
│   ├── math3d.c
│   ├── renderer.c
│   └── lighting.c
├── models/
│   ├── generate_cube.c
│   ├── generate_cylinder.c
│   ├── generate_pyramid.c
│   └── generate_soccer_ball.c
├── demo/
│   └── main.c
├── tests/
│   ├── test_math.c
│   ├── test_pipeline.c
│   ├── math_tests/
│   │   └── test_math.exe
│   └── visual_tests/
│       ├── test_pipeline.exe
│       └── visuals/
│           ├── test_cube/
│           └── test_soccer/
├── build/
│   ├── main.exe
│   ├── fully_rendered_scene/
│   └── clockface/
└── Makefile
```

---

## ⚙️ Building and Usage

This project uses a `Makefile` to automate compilation, execution, and animation generation.

### 🧱 Building Executables

Run the following command to build all components:

```bash
make
```

This creates:

- `build/main.exe` — Main demo program
- `tests/math_tests/test_math.exe` — Unit tests for math utilities
- `tests/visual_tests/test_pipeline.exe` — Visual rendering pipeline test

---

### ▶️ Running Programs

To run the main demo and generate frames into `fully_rendered_scene/`:

```bash
make run_main
```

To run the math unit tests:

```bash
make run_math
```

To run the rendering pipeline (e.g., soccer ball test):

```bash
make run_pipeline
```

Output frames will be saved in:

```
visuals/test_soccer/
```

---

### 🎮 Generating GIF Animations

Once PNG frames are rendered, you can convert them into animated GIFs using ImageMagick.

#### Main scene animation:

```bash
make gif_main
```

> Output: `fully_rendered_scene/animation.gif`

#### Soccer ball test animation:

```bash
make gif_soccer
```

> Output: `visuals/test_soccer/test_soccer.gif`

#### To generate both:

```bash
make all_gifs
```

> ⚠️ **Note**: Requires [ImageMagick](https://imagemagick.org/) (`magick` command must be available in your terminal or CMD).

---

### 🪚 Cleanup

Remove all generated `.exe` files:

```bash
make clean
```

Remove all generated `.gif` files:

```bash
make clean_gif
```

---

## 🧐 Functionality and Header Files

### 📌 canvas.h

Low-level pixel and depth buffer management.

#### Types:

- `canvas_t`: holds width, height, grayscale pixels, and z-buffer.

#### Functions:

- `canvas_t* create_canvas(int width, int height)`
- `void destroy_canvas(canvas_t* canvas)`
- `void clear_canvas(canvas_t* canvas, float value)`
- `void set_pixel_f(...)`
- `void draw_line_f(...)`
- `void draw_line_2d(...)`
- `void save_canvas_ppm(...)`
- `void save_canvas_png(...)`

---

### 🧾 math3d.h

Mathematics for 3D transformations.

#### Types:

- `vec3_t`: 3D vectors
- `mat4_t`: 4x4 transformation matrices

#### Functions:

- Vector: `vec3_normalize`, `vec3_cross`, `vec3_dot`, `vec3_slerp`, etc.
- Matrix: `mat4_identity`, `mat4_translate`, `mat4_scale`, `mat4_rotate_xyz`, `mat4_frustum_asymmetric`, `look_at_matrix`, etc.

---

### 🥵 renderer.h

Handles projecting and rendering 3D meshes.

#### Types:

- `mesh_t`: Structure holding `vertices`, `edges`, and `faces`.

#### Functions:

- `project_vertex(...)`
- `clip_to_circular_viewport(...)`
- `render_edges_only(...)`
- `render_edges_from_faces_with_lighting_multi(...)`
- `render_faces_with_lighting_multi(...)`
- `init_original_edges(...)`

---

### 💡 lighting.h

Basic Lambertian lighting support.

#### Functions:

- `float lambert_brightness_multi(...)`

---

### ✨ animation.h

Provides cubic Bézier animation support.

#### Functions:

- `vec3_t bezier(...)`

---

## 💻 Sample Usage (main.c)

```c
#include <math.h>
#include "canvas.h"
#include "math3d.h"
#include "renderer.h"
#include "lighting.h"
#include "animation.h"

#define PI 3.14159265f

int main() {
    int width = 800, height = 800;
    canvas_t* canvas = create_canvas(width, height);
    clear_canvas(canvas, 0.0f);

    mesh_t soccerball;
    generate_soccer_ball(&soccerball);

    vec3_t camera_pos = { 0, 0, -5 };
    vec3_t target = { 0, 0, 0 };
    vec3_t up = { 0, 1, 0 };
    mat4_t view = look_at_matrix(camera_pos, target, up);

    float near = 0.1f, far = 100.0f, fov = 60.0f * (PI / 180.0f);
    float aspect = (float)width / (float)height;
    float top = near * tanf(fov / 2.0f);
    float bottom = -top;
    float right = top * aspect;
    float left = -right;
    mat4_t proj = mat4_frustum_asymmetric(left, right, bottom, top, near, far);

    vec3_t light_dirs[2] = {
        vec3_normalize((vec3_t){ 1, 1, -1 }),
        vec3_normalize((vec3_t){ -1, -1, -1 })
    };

    float angle = PI / 3.0f;
    mat4_t model = mat4_rotate_y(angle);
    model = mat4_mul(&model, &mat4_scale(0.4f, 0.4f, 0.4f));

    mat4_t mvp = mat4_mul(&proj, &view);
    mvp = mat4_mul(&mvp, &model);

    render_faces_with_lighting_multi(canvas, &soccerball, &mvp, &model, light_dirs, 2);
    save_canvas_ppm(canvas, "soccerball_frame.ppm");

    destroy_canvas(canvas);
    free(soccerball.vertices);
    free(soccerball.faces);

    return 0;
}
```

To render a 3D object, use one of the geometry generators like `generate_soccer_ball()` to populate the mesh structure. These are implemented in the `models/` folder. Meshes are **not** loaded from files — they are **hardcoded in C**.

---

## 📄 License

This project is free to use, modify, and share. No formal license is applied — use it for academic, personal, or commercial work. Attribution is appreciated but not required.

