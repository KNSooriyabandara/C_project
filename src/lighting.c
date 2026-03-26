#include "lighting.h"
#include <math.h>

float lambert_brightness_multi(vec3_t edge_dir, const vec3_t* light_dirs, int light_count) {
    edge_dir = vec3_normalize_fast(edge_dir);

    float total = 0.0f;
    for (int i = 0; i < light_count; ++i) {
        vec3_t light_dir = vec3_normalize_fast(light_dirs[i]);
        float dot = edge_dir.x * light_dir.x + edge_dir.y * light_dir.y + edge_dir.z * light_dir.z;
        if (dot > 0.0f)
            total += dot;
    }
    if(total<0.2)total=0.2;
    if (total > 1.0f) total = 1.0f; // optional clamping
    return total;
}
