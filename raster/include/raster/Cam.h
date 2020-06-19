#pragma once

#include "raster/Transform.h"

struct Cam
{
    float fov;
    float _near;
    float _far;

    Transform t;
};

inline Cam
cam_new()
{
    Cam self;
    self.fov   = 90;
    self._near = 0.1f;
    self._far  = 100;
    self.t     = transform_new();
    return self;
}

inline Mat4
cam_view_proj(const Cam &self)
{
    return
        mat4_inverse(transform_mat(self.t)) *
        mat4_proj_prespective(90, 0.1f, 100.0f);
}
