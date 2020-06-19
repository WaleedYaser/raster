#pragma once

#include "raster/Vec3.h"

#include <stdint.h>
#include <math.h>
#include <assert.h>

#define PI 3.14159265359

struct Mat4
{
    float m[4][4];

    inline const float *
    operator[](uint8_t i) const
    {
        return m[i];
    }

    inline float *
    operator[](uint8_t i)
    {
        return m[i];
    }
};

inline Mat4
operator*(const Mat4 &A, const Mat4 &B)
{
    Mat4 M;
    for (uint8_t i = 0; i < 4; ++i)
    {
        for (uint8_t j = 0; j < 4; ++j)
        {
            M[i][j] =
                A[i][0] * B[0][j] +
                A[i][1] * B[1][j] +
                A[i][2] * B[2][j] +
                A[i][3] * B[3][j];
        }
    }
    return M;
}

inline Mat4
operator*(const Mat4 &M, float s)
{
    return Mat4{{
        {M[0][0] * s, M[0][1] * s, M[0][2] * s, M[0][3] * s},
        {M[1][0] * s, M[1][1] * s, M[1][2] * s, M[1][3] * s},
        {M[2][0] * s, M[2][1] * s, M[2][2] * s, M[2][3] * s},
        {M[3][0] * s, M[3][1] * s, M[3][2] * s, M[3][3] * s}}};
}

inline Mat4
operator*(float s, const Mat4 &M)
{
    return M * s;
}

inline Mat4
mat4_identity()
{
    return Mat4{{
        {1, 0, 0, 0},
        {0, 1, 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 1}}};
}

inline Mat4
mat4_proj_prespective(float aov, float _near, float _far)
{
    Mat4 M = {};

    float scale = 1 / tan(aov * 0.5 * PI / 180);
    M[0][0] = scale;
    M[1][1] = scale;
    M[2][2] = -_far / (_far - _near);
    M[3][2] = -_far * _near / (_far - _near);
    M[2][3] = -1;
    M[3][3] = 0;

    return M;
}

inline Mat4
mat4_transpose(const Mat4 &M)
{
    return Mat4{{
        {M[0][0], M[1][0], M[2][0], M[3][0]},
        {M[0][1], M[1][1], M[2][1], M[3][1]},
        {M[0][2], M[1][2], M[2][2], M[3][2]},
        {M[0][3], M[1][3], M[2][3], M[3][3]}}};
}

inline Mat4
mat4_inverse(const Mat4 &M)
{
    // clone A
    Mat4 A = M;
    // result
    Mat4 R = mat4_identity();
    for (uint8_t column = 0; column < 4; ++column)
    {
        // swap row in case out pivot point is not working
        if (A[column][column] == 0.0f)
        {
            uint8_t row_max = column;
            for (uint8_t row = 0; row < 4; ++row)
                if (fabsf(A[row][column]) > fabsf(A[row_max][column]))
                    row_max = row;
            if (row_max == column)
            {
                // TODO: singular matrix
                assert(false && "singular matrix");
            }
            // Swap rows
            else
            {
                for (uint8_t j = 0; j < 4; ++j)
                {
                    float tmp = A[column][j];
                    A[column][j] = A[row_max][j];
                    A[row_max][j] = tmp;

                    tmp = R[column][j];
                    R[column][j] = R[row_max][j];
                    R[row_max][j] = tmp;
                }
            }
        }

        // Set each row in the column to 0
        for (uint8_t row = 0; row < 4; ++row)
        {
            if (row != column)
            {
                float coeff = A[row][column] / A[column][column];
                if (coeff != 0.0f)
                {
                    for (uint8_t j = 0; j < 4; ++j)
                    {
                        A[row][j] -= coeff * A[column][j];
                        R[row][j] -= coeff * R[column][j];
                    }
                    // Set the element to 0 for safety
                    A[row][column] = 0.0f;
                }
            }
        }
    }

    // Set each element of the diagonal to 1
    for (uint8_t row = 0; row < 4; ++row)
    {
        for (uint8_t column = 0; column < 4; ++column)
        {
            R[row][column] /= A[row][row];
        }
    }
    return R;
}

inline Mat4
mat4_translate(float dx, float dy, float dz)
{
    Mat4 M  = mat4_identity();
    M[3][0] = dx;
    M[3][1] = dy;
    M[3][2] = dz;
    return M;
}

inline Mat4
mat4_rotate_x(float theta)
{
    Mat4 M  = mat4_identity();
    M[1][1] = cosf(theta);
    M[1][2] = sinf(theta);
    M[2][1] = -sinf(theta);
    M[1][2] = cosf(theta);
    return M;
}

inline Mat4
mat4_rotate_y(float theta)
{
    Mat4 M  = mat4_identity();
    M[0][0] = cosf(theta);
    M[0][2] = -sinf(theta);
    M[2][0] = sinf(theta);
    M[2][2] = cosf(theta);
    return M;
}

inline Mat4
mat4_rotate_z(float theta)
{
    Mat4 M  = mat4_identity();
    M[0][0] = cosf(theta);
    M[0][1] = sinf(theta);
    M[1][0] = -sinf(theta);
    M[1][1] = cosf(theta);
    return M;
}

inline Mat4
mat4_scale(float scale_x, float scale_y, float scale_z)
{
    Mat4 M  = mat4_identity();
    M[0][0] = scale_x;
    M[1][1] = scale_y;
    M[2][2] = scale_z;
    return M;
}

inline Vec3
mat4_transform_point(const Mat4 &M, const Vec3 &p)
{
    return Vec3{
        p.x * M[0][0] + p.y * M[1][0] + p.z * M[2][0] + M[3][0],
        p.x * M[0][1] + p.y * M[1][1] + p.z * M[2][1] + M[3][1],
        p.x * M[0][2] + p.y * M[1][2] + p.z * M[2][2] + M[3][2]};
}


inline Vec3
mat4_transform_vec(const Mat4 &M, const Vec3 &v)
{
    return Vec3{
        v.x * M[0][0] + v.y * M[1][0] + v.z * M[2][0],
        v.x * M[0][1] + v.y * M[1][1] + v.z * M[2][1],
        v.x * M[0][2] + v.y * M[1][2] + v.z * M[2][2]};
}

inline Vec3
mat4_transform_normal(const Mat4 &M, const Vec3 &n)
{
    // we don't care about translation so lets assume that we have
    // M = R * S where R is rotation matrix and S is scaling matrix
    // R is orthogonal so transpose is the same for the inverse so
    // taking traospose of the inverse doesn't change any thing
    // S is diagonal matrix so transposing it doesn't change any
    // thing, but inversing it will apply the inverse scaling so
    // the normals wont be sheered
    Mat4 M_ = mat4_transpose(mat4_inverse(M));
    return mat4_transform_vec(M_, n);
}

inline Vec3
mat4_transform_proj_point(const Mat4 &M, const Vec3 &p)
{
    float w = p.x * M[0][3] + p.y * M[1][3] + p.z * M[2][3] + M[3][3];
    w = 1 / w;
    return Vec3{
        (p.x * M[0][0] + p.y * M[1][0] + p.z * M[2][0] + M[3][0]) * w,
        (p.x * M[0][1] + p.y * M[1][1] + p.z * M[2][1] + M[3][1]) * w,
        (p.x * M[0][2] + p.y * M[1][2] + p.z * M[2][2] + M[3][2]) * w};
}

inline Vec3
mat4_axis_x(const Mat4 &M)
{
    return Vec3{M[0][0], M[0][1], M[0][2]};
}

inline Vec3
mat4_axis_y(const Mat4 &M)
{
    return Vec3{M[1][0], M[1][1], M[1][2]};
}

inline Vec3
mat4_axis_z(const Mat4 &M)
{
    return Vec3{M[2][0], M[2][1], M[2][2]};
}
