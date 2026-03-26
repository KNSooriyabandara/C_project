#ifndef LIGHTING_H
#define LIGHTING_H

#include "math3d.h"

float lambert_brightness_multi(vec3_t edge_dir, const vec3_t* light_dirs, int light_count);

#endif
