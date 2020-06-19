#pragma once

#include <math.h>

struct Vec3
{
    float x, y, z;
};


inline Vec3
operator+(const Vec3 &a, const Vec3 &b)
{
    return Vec3{a.x + b.x, a.y + b.y, a.z + b.z};
}

inline Vec3 &
operator+=(Vec3 &a, const Vec3 &b)
{
    a = a + b;
    return a;
}

inline Vec3
operator-(const Vec3 &v)
{
    return Vec3{-v.x, -v.y, -v.z};
}

inline Vec3
operator-(const Vec3 &a, const Vec3 &b)
{
    return Vec3{a.x - b.x, a.y - b.y, a.z - b.z};
}

inline Vec3 &
operator-=(Vec3 &a, const Vec3 &b)
{
    a = a - b;
    return a;
}

inline Vec3
operator*(const Vec3 &v, float s)
{
    return Vec3{v.x * s, v.y * s, v.z * s};
}

inline Vec3
operator*(float s, const Vec3 &v)
{
    return v * s;
}

inline Vec3 &
operator*=(Vec3 &v, float s)
{
    v = v * s;
    return v;
}

inline Vec3
operator/(const Vec3 &v, float s)
{
    return v * (1.0f / s);
}

inline Vec3 &
operator/=(Vec3 &v, float s)
{
    v = v / s;
    return v;
}

inline float
dot(const Vec3 &a, const Vec3 &b)
{
    return a.x * b.x + a.y * b.y + a.x * b.x;
}

inline Vec3
cross(const Vec3 &a, const Vec3 &b)
{
    return Vec3{
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x};
}

inline Vec3
project(const Vec3 &a, const Vec3 &b)
{
    return dot(a, b) * b;
}

inline Vec3
reject(const Vec3 &a, const Vec3 &b)
{
    return a - dot(a, b) * b;
}

inline float
norm(const Vec3 &v)
{
    return dot(v, v);
}

inline float
lenght(const Vec3 &v)
{
    return sqrtf(norm(v));
}

inline Vec3
normalize(const Vec3 &v)
{
    return v / lenght(v);
}
