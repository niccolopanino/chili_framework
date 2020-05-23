#pragma once
#include "Vec2.h"
#include "Vec3.h"
#include "Vec4.h"
#include "Colors.h"
#include "BaseVertexShader.h"
#include "DefaultGeometryShader.h"
#include "Surface.h"
#include "ChiliMath.h"
#include <algorithm>
#include <memory>
#include <string>

template<typename DiffuseParams>
class VertexLightTexEffect
{
public:
    class Vertex
    {
    public:
        Vertex() = default;
        Vertex(const Vec4f &pos) : m_pos(pos) { }
        Vertex(const Vec4f &pos, const Vertex &src) : m_pos(pos), m_n(src.m_n), m_tc(src.m_tc) { }
        Vertex(const Vec4f &pos, const Vec3f &n, const Vec2f &tc) : m_pos(pos), m_n(n), m_tc(tc) { }
    public:
        Vec4f m_pos;
        Vec3f m_n;
        Vec2f m_tc;
    };
    class VSOut
    {
    public:
        VSOut() = default;
        VSOut(const Vec4f &pos) : m_pos(pos) { }
        VSOut(const Vec4f &pos, const VSOut &src) : m_pos(pos), m_l(src.m_l), m_tc(src.m_tc) { }
        VSOut(const Vec4f &pos, const Vec3f &n, const Vec2f &tc) : m_pos(pos), m_l(n), m_tc(tc) { }
        VSOut operator+(const VSOut &rhs) const;
        VSOut &operator+=(const VSOut &rhs) { return *this = *this + rhs; }
        VSOut operator-(const VSOut &rhs) const;
        VSOut &operator-=(const VSOut &rhs) { return *this = *this - rhs; }
        VSOut operator*(float rhs) const { return VSOut(m_pos * rhs, m_l * rhs, m_tc * rhs); }
        VSOut &operator*=(float rhs) { return *this = *this * rhs; }
        VSOut operator/(float rhs) const { return VSOut(m_pos / rhs, m_l / rhs, m_tc / rhs); }
        VSOut &operator/=(float rhs) { return *this = *this / rhs; }
    public:
        Vec4f m_pos;
        Vec3f m_l;
        Vec2f m_tc;
    };
    class VertexShader : public BaseVertexShader<VSOut>
    {
    public:
        typename BaseVertexShader<VSOut>::Output operator()(const Vertex &input) const
        {
            // transform mech vertex position before lighting calc
            const auto world_pos = input.m_pos * m_world_view;
            // vertex to light data
            const auto v_to_l = m_light_pos - world_pos;
            const auto dist = v_to_l.len();
            const Vec3f dir = v_to_l / dist;
            // calculate attenuation
            const auto att = 1.f / (DiffuseParams::k_const_att + DiffuseParams::k_lin_att * dist
                + DiffuseParams::k_quad_att * sq(dist));
            // calculate intennsity based on angle of incidence and attenuation
            const auto d = m_light_diff * att * std::max(0.f,
                Vec3f::dot(static_cast<Vec3f>(Vec4f(input.m_n, 0.f) * m_world_view), dir));
            // add diffuse, ambient, filter by material color, saturate and scale
            const auto l = d + m_light_amb;
            return Output(input.m_pos * m_world_view_proj, l, input.m_tc);
        }
        void set_diffuse_light(const Vec3f &c) { m_light_diff = c; }
        void set_ambient_light(const Vec3f &c) { m_light_amb = c; }
        void set_light_pos(const Vec4f &pos) { m_light_pos = pos; }
    private:
        Vec4f m_light_pos;
        Vec3f m_light_diff;
        Vec3f m_light_amb;
    };
    typedef DefaultGeometryShader<typename VertexShader::Output> GeometryShader;
    class PixelShader
    {
    public:
        template<typename I>
        Color operator()(const I &input) const;
        void bind_texture(const Surface &tex);
    private:
        const Surface *m_tex = nullptr;
        unsigned int m_tex_width = 0u;
        unsigned int m_tex_height = 0u;
    };
public:
    VertexShader m_vs;
    GeometryShader m_gs;
    PixelShader m_ps;
};

template<typename DiffuseParams>
inline typename VertexLightTexEffect<DiffuseParams>::VSOut
VertexLightTexEffect<DiffuseParams>::VSOut::operator+(const VSOut &rhs) const
{
    return VSOut(m_pos + rhs.m_pos, m_l + rhs.m_l, m_tc + rhs.m_tc);
}

template<typename DiffuseParams>
inline typename VertexLightTexEffect<DiffuseParams>::VSOut
VertexLightTexEffect<DiffuseParams>::VSOut::operator-(const VSOut &rhs) const
{
    return VSOut(m_pos - rhs.m_pos, m_l - rhs.m_l, m_tc - rhs.m_tc);
}

template<typename DiffuseParams>
template<typename I>
inline Color VertexLightTexEffect<DiffuseParams>::PixelShader::operator()(const I &input) const
{
    const auto mat_color = Vec3f(m_tex->get_pixel(
        unsigned int(input.m_tc.m_x * m_tex_width + .5f) % m_tex_width,
        unsigned int(input.m_tc.m_y * m_tex_height + .5f) % m_tex_height
    )) / 255.f;
    return Color((mat_color * input.m_l).get_saturated() * 255.f);
}

template<typename DiffuseParams>
inline void VertexLightTexEffect<DiffuseParams>::PixelShader::bind_texture(const Surface &tex)
{
    m_tex = &tex;
    m_tex_width = m_tex->get_width();
    m_tex_height = m_tex->get_height();
}
