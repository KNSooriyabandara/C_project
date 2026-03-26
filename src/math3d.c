#include "math3d.h"
#include <math.h>

vec3_t vec3_from_spherical(float r, float theta, float phi) {
    vec3_t v;
    v.x = r * sinf(theta) * cosf(phi);
    v.y = r * sinf(theta) * sinf(phi);
    v.z = r * cosf(theta);
    return v;
}

vec3_t vec3_normalize_fast(vec3_t v) {
    float lengthSq = v.x * v.x + v.y * v.y + v.z * v.z;
    float invSqrt = 1.0f / sqrtf(lengthSq); // You can replace this with fast inverse sqrt if you like
    v.x *= invSqrt;
    v.y *= invSqrt;
    v.z *= invSqrt;
    return v;
}

vec3_t vec3_slerp(vec3_t a, vec3_t b, float t) {
    float dot = a.x * b.x + a.y * b.y + a.z * b.z;
    dot = fminf(fmaxf(dot, -1.0f), 1.0f); // Clamp dot between -1 and 1
    float theta = acosf(dot);
    float sin_theta = sinf(theta);

    if (sin_theta == 0.0f) return a;

    float w1 = sinf((1 - t) * theta) / sin_theta;
    float w2 = sinf(t * theta) / sin_theta;

    vec3_t result;
    result.x = w1 * a.x + w2 * b.x;
    result.y = w1 * a.y + w2 * b.y;
    result.z = w1 * a.z + w2 * b.z;
    return result;
}
mat4_t mat4_identity() {
    mat4_t m = {0};
    m.m[0] = 1; m.m[5] = 1; m.m[10] = 1; m.m[15] = 1;
    return m;
}
mat4_t mat4_translate(float tx, float ty, float tz) {
    mat4_t m = mat4_identity();
    m.m[12] = tx;
    m.m[13] = ty;
    m.m[14] = tz;
    return m;
}
mat4_t mat4_scale(float sx, float sy, float sz) {
    mat4_t m = {0};
    m.m[0] = sx; m.m[5] = sy; m.m[10] = sz; m.m[15] = 1;
    return m;
}
mat4_t mat4_rotate_xyz(float rx, float ry, float rz) {
    // Convert degrees to radians if necessary.
    float cx = cosf(rx), sx = sinf(rx);
    float cy = cosf(ry), sy = sinf(ry);
    float cz = cosf(rz), sz = sinf(rz);

    mat4_t m = mat4_identity();

    // Rotation order: Z * Y * X
    m.m[0] = cy * cz;
    m.m[1] = cy * sz;
    m.m[2] = -sy;

    m.m[4] = sx * sy * cz - cx * sz;
    m.m[5] = sx * sy * sz + cx * cz;
    m.m[6] = sx * cy;

    m.m[8] = cx * sy * cz + sx * sz;
    m.m[9] = cx * sy * sz - sx * cz;
    m.m[10] = cx * cy;

    m.m[15] = 1;
    return m;
}
mat4_t mat4_frustum_asymmetric(float left, float right, float bottom, float top, float near, float far) {
    mat4_t m = {0};

    m.m[0] = (2.0f * near) / (right - left);
    m.m[5] = (2.0f * near) / (top - bottom);
    m.m[8] = (right + left) / (right - left);
    m.m[9] = (top + bottom) / (top - bottom);
    m.m[10] = -(far + near) / (far - near);
    m.m[11] = -1.0f;
    m.m[14] = -(2.0f * far * near) / (far - near);

    return m;
}
mat4_t mat4_mul(const mat4_t* a, const mat4_t* b) {
    mat4_t result = {0};
    for (int col = 0; col < 4; col++) {
        for (int row = 0; row < 4; row++) {
            for (int k = 0; k < 4; k++) {
                result.m[col * 4 + row] += a->m[k * 4 + row] * b->m[col * 4 + k];
            }
        }
    }
    return result;
}
// Subtract two vec3 vectors: result = a - b
vec3_t vec3_sub(vec3_t a, vec3_t b) {
    vec3_t result = {a.x - b.x, a.y - b.y, a.z - b.z};
    return result;
}

// Dot product of two vec3 vectors
float vec3_dot(vec3_t a, vec3_t b) {
    return a.x*b.x + a.y*b.y + a.z*b.z;
}

// Cross product of two vec3 vectors
vec3_t vec3_cross(vec3_t a, vec3_t b) {
    vec3_t result = {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
    return result;
}
vec3_t vec3_normalize(vec3_t v) {
    float len = sqrtf(v.x*v.x + v.y*v.y + v.z*v.z);
    if (len > 0.00001f) {
        vec3_t res = {v.x / len, v.y / len, v.z / len};
        return res;
    }
    return v;
}
mat4_t look_at_matrix(vec3_t eye, vec3_t target, vec3_t up) {
    vec3_t zaxis = vec3_normalize(vec3_sub(eye, target));   // camera forward (view direction)
    vec3_t xaxis = vec3_normalize(vec3_cross(up, zaxis));   // camera right
    vec3_t yaxis = vec3_cross(zaxis, xaxis);                // camera up corrected

    mat4_t view = {0};

    view.m[0] = xaxis.x;
    view.m[1] = yaxis.x;
    view.m[2] = zaxis.x;
    view.m[3] = 0;

    view.m[4] = xaxis.y;
    view.m[5] = yaxis.y;
    view.m[6] = zaxis.y;
    view.m[7] = 0;

    view.m[8] = xaxis.z;
    view.m[9] = yaxis.z;
    view.m[10] = zaxis.z;
    view.m[11] = 0;

    view.m[12] = -vec3_dot(xaxis, eye);
    view.m[13] = -vec3_dot(yaxis, eye);
    view.m[14] = -vec3_dot(zaxis, eye);
    view.m[15] = 1;

    return view;
}
vec3_t mat4_mul_vec3(const mat4_t* m, vec3_t v) {
    vec3_t r;
    r.x = m->m[0]*v.x + m->m[4]*v.y + m->m[8]*v.z;
    r.y = m->m[1]*v.x + m->m[5]*v.y + m->m[9]*v.z;
    r.z = m->m[2]*v.x + m->m[6]*v.y + m->m[10]*v.z;
    return r;
}
mat4_t mat4_transpose(const mat4_t* m) {
    mat4_t t;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            t.m[i + j*4] = m->m[j + i*4];
        }
    }
    return t;
}


mat4_t mat4_rotate_y(float angle) {
    mat4_t m = {0};
    float c = cosf(angle);
    float s = sinf(angle);

    m.m[0] = c;    m.m[1] = 0; m.m[2] = -s;   m.m[3] = 0;
    m.m[4] = 0;    m.m[5] = 1; m.m[6] = 0;    m.m[7] = 0;
    m.m[8] = s;    m.m[9] = 0; m.m[10] = c;   m.m[11] = 0;
    m.m[12] = 0;   m.m[13] = 0; m.m[14] = 0;  m.m[15] = 1;

    return m;
}
vec3_t mat4_transform_normal(mat4_t m, vec3_t v) {
    vec3_t result;
    result.x = m.m[0]*v.x + m.m[4]*v.y + m.m[8]*v.z;
    result.y = m.m[1]*v.x + m.m[5]*v.y + m.m[9]*v.z;
    result.z = m.m[2]*v.x + m.m[6]*v.y + m.m[10]*v.z;
    return result;
}
mat4_t mat4_rotate_z(float angle) {
    mat4_t m = {0};
    float c = cosf(angle);
    float s = sinf(angle);

    m.m[0] = c;  m.m[1] = -s; m.m[2] = 0;  m.m[3] = 0;
    m.m[4] = s;  m.m[5] = c;  m.m[6] = 0;  m.m[7] = 0;
    m.m[8] = 0;  m.m[9] = 0;  m.m[10] = 1; m.m[11] = 0;
    m.m[12] = 0; m.m[13] = 0; m.m[14] = 0; m.m[15] = 1;

    return m;
}
mat4_t mat4_rotate_x(float angle) {
    mat4_t m = {0};
    float c = cosf(angle);
    float s = sinf(angle);

    m.m[0] = 1;  m.m[1] = 0;  m.m[2] = 0;   m.m[3] = 0;
    m.m[4] = 0;  m.m[5] = c;  m.m[6] = -s;  m.m[7] = 0;
    m.m[8] = 0;  m.m[9] = s;  m.m[10] = c;  m.m[11] = 0;
    m.m[12] = 0; m.m[13] = 0; m.m[14] = 0;  m.m[15] = 1;

    return m;
}
#include <math.h>
#include "math3d.h"

quat_t quat_slerp(quat_t a, quat_t b, float t) {
        // Compute the cosine of the angle between the two vectors.
    float dot = a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w;
    if (dot < 0.0f) {
        b.x = -b.x; b.y = -b.y; b.z = -b.z; b.w = -b.w;
        dot = -dot;
    }

    const float DOT_THRESHOLD = 0.9995f;
    if (dot > DOT_THRESHOLD) {
        // If the inputs are too close for comfort, linearly interpolate and normalize the result.
        quat_t result = {
            a.x + t*(b.x - a.x),
            a.y + t*(b.y - a.y),
            a.z + t*(b.z - a.z),
            a.w + t*(b.w - a.w)
        };
        // Normalize quaternion
        float inv_len = 1.0f / sqrtf(result.x*result.x + result.y*result.y + result.z*result.z + result.w*result.w);
        result.x *= inv_len;
        result.y *= inv_len;
        result.z *= inv_len;
        result.w *= inv_len;
        return result;
    }

    // Acos the angle between quaternions
    float theta_0 = acosf(dot);
    float theta = theta_0 * t;
    float sin_theta = sinf(theta);
    float sin_theta_0 = sinf(theta_0);

    float s0 = cosf(theta) - dot * sin_theta / sin_theta_0;
    float s1 = sin_theta / sin_theta_0;

    quat_t result = {
        (s0 * a.x) + (s1 * b.x),
        (s0 * a.y) + (s1 * b.y),
        (s0 * a.z) + (s1 * b.z),
        (s0 * a.w) + (s1 * b.w)
    };

    return result;
    // ... full slerp function as shown before ...
}

quat_t quat_identity(void) {
    quat_t q = {0, 0, 0, 1};
    return q;
}

quat_t quat_from_axis_angle(vec3_t axis, float angle_rad) {
    float half = angle_rad / 2.0f;
    float s = sinf(half);
    quat_t q = {
        axis.x * s,
        axis.y * s,
        axis.z * s,
        cosf(half)
    };
    return q;
}

mat4_t mat4_from_quat(quat_t q) {
    mat4_t m = {0};
   
    float xx = q.x * q.x;
    float yy = q.y * q.y;
    float zz = q.z * q.z;
    float xy = q.x * q.y;
    float xz = q.x * q.z;
    float yz = q.y * q.z;
    float wx = q.w * q.x;
    float wy = q.w * q.y;
    float wz = q.w * q.z;

    m.m[0]  = 1.0f - 2.0f * (yy + zz);
    m.m[1]  = 2.0f * (xy - wz);
    m.m[2]  = 2.0f * (xz + wy);
    m.m[3]  = 0.0f;

    m.m[4]  = 2.0f * (xy + wz);
    m.m[5]  = 1.0f - 2.0f * (xx + zz);
    m.m[6]  = 2.0f * (yz - wx);
    m.m[7]  = 0.0f;

    m.m[8]  = 2.0f * (xz - wy);
    m.m[9]  = 2.0f * (yz + wx);
    m.m[10] = 1.0f - 2.0f * (xx + yy);
    m.m[11] = 0.0f;

    m.m[12] = 0.0f;
    m.m[13] = 0.0f;
    m.m[14] = 0.0f;
    m.m[15] = 1.0f;

    return m;
    // ... full conversion as shown before ...
    return m;
}
