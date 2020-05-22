#pragma once
#include <cmath>

constexpr float PI = 3.14159265f;
constexpr double PI_D = 3.1415926535897932;

template<typename T>
constexpr T sq(const T &x)
{
    return x * x;
}

template<typename T>
inline T wrap_angle(T theta)
{
    const T mod = fmod(theta, (T)2 * (T)PI_D);
    return (mod > (T)PI_D) ? (mod - (T)2 * (T)PI_D) : mod;
}

template<typename T>
constexpr T interpolate(const T &src, const T &dst, float alpha)
{
    return src + (dst - src) * alpha;
}

template<typename T>
constexpr T to_rad(T deg)
{
    return deg * PI / (T)180;
}
