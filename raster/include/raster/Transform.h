#pragma once

#include "raster/Vec3.h"
#include "raster/Mat4.h"

struct Transform
{
    Vec3 position;
    Vec3 rotation;
    Vec3 scaling;
};

inline Transform
transform_new()
{
    return Transform{
        Vec3{0, 0, 0},
        Vec3{0, 0, 0},
        Vec3{1, 1, 1}};
}

inline Mat4
transform_mat(const Transform &self)
{
    return
        mat4_scale(self.scaling.x, self.scaling.y, self.scaling.z) *
        mat4_rotate_x(self.rotation.x) *
        mat4_rotate_y(self.rotation.y) *
        mat4_rotate_z(self.rotation.z) *
        mat4_translate(self.position.x, self.position.y, self.position.z);
}
