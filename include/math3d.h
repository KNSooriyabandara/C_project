// math3d.h
#ifndef MATH3D_H
#define MATH3D_H

typedef struct {
    union {
        struct { float x, y, z; };
        struct { float r, theta, phi; };
    };
} vec3_t;

vec3_t vec3_from_spherical(float r, float theta, float phi);
vec3_t vec3_normalize_fast(vec3_t v);
vec3_t vec3_slerp(vec3_t a, vec3_t b, float t);
vec3_t vec3_cross(vec3_t a, vec3_t b);
float vec3_dot(vec3_t a, vec3_t b);
vec3_t vec3_sub(vec3_t a, vec3_t b);
vec3_t vec3_normalize(vec3_t v);

typedef struct {
    float m[16];  // column-major order (OpenGL style)
} mat4_t;

mat4_t mat4_identity();
mat4_t mat4_translate(float tx, float ty, float tz);
mat4_t mat4_scale(float sx, float sy, float sz);
mat4_t mat4_rotate_xyz(float rx, float ry, float rz);
mat4_t mat4_frustum_asymmetric(float left, float right, float bottom, float top, float near, float far);
mat4_t mat4_mul(const mat4_t* a, const mat4_t* b);
mat4_t look_at_matrix(vec3_t eye, vec3_t target, vec3_t up);
vec3_t vec3_normalize(vec3_t v);
vec3_t mat4_mul_vec3(const mat4_t* m, vec3_t v);
mat4_t mat4_transpose(const mat4_t* m);
mat4_t mat4_rotate_y(float angle);
vec3_t mat4_transform_normal(mat4_t m, vec3_t v);
mat4_t mat4_rotate_z(float angle);
mat4_t mat4_rotate_x(float angle);

typedef struct {
    float x, y, z, w;
} quat_t;

quat_t quat_slerp(quat_t a, quat_t b, float t);
quat_t quat_identity(void);
quat_t quat_from_axis_angle(vec3_t axis, float angle_rad);
mat4_t mat4_from_quat(quat_t q);

#endif
