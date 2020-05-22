#pragma once
#include "Vec3.h"
#include "Colors.h"
#include "ChiliMath.h"
#include <algorithm>

struct DefaultPointDiffuseParams
{
    static constexpr float k_lin_att = 1.f;
    static constexpr float k_quad_att = 2.619f;
    static constexpr float k_const_att = .382f;
};

struct DefaultSpecularParams
{
    static constexpr float k_spec_pow = 30.f;
    static constexpr float k_spec_in = .6f;
};

template<typename DP = DefaultPointDiffuseParams, typename SP = DefaultSpecularParams>
class BasePhongShader
{
public:
    template<typename I>
    Color shade(const I &input, const Vec3f &mat_color) const
    {
        // re-normalize interpolated surface normal
        const auto surf_norm = input.m_n.get_normalized();
        // vertex to light data
        const auto v_to_l = m_light_pos - input.m_world_pos;
        const auto dist = v_to_l.len();
        const auto dir = v_to_l / dist;
        // calculate attenuation
        const float att = 1.f
            / (DP::k_const_att + DP::k_lin_att * dist + DP::k_quad_att * sq(dist));
        // calculate intensity based on angle of incidence and attenuation
        const auto d = m_light_diff * att * std::max(0.f, Vec3f::dot(surf_norm, dir));
        // reflected light vector
        const auto r = surf_norm * Vec3f::dot(v_to_l, surf_norm) * 2.f - v_to_l;
        // calculate specular intensity based on angle between viewing vector
        // and reflection vector, narrow with power function
        const auto s = m_light_diff * SP::k_spec_in * std::pow(std::max(0.f,
            Vec3f::dot(-r.get_normalized(), input.m_world_pos.get_normalized())), SP::k_spec_pow);
        // add diffuse, ambient, filter by material color, saturate and scale
        return Color((mat_color * (d + m_light_amb + s)).saturate() * 255.f);
    }
    void set_diffuse_light(const Vec3f &c) { m_light_diff = c; }
    void set_ambient_light(const Vec3f &c) { m_light_amb = c; }
    void set_light_pos(const Vec3f &pos) { m_light_pos = pos; }
private:
    Vec3f m_light_pos = Vec3f(0.f, 0.f, .5f);
    Vec3f m_light_diff = Vec3f(1.f, 1.f, 1.f);
    Vec3f m_light_amb = Vec3f(.1f, .1f, .1f);
};
