#pragma once
#include "Vec2.h"
#include "Vec3.h"
#include "Vec4.h"
#include "Colors.h"
#include "BaseVertexShader.h"
#include "DefaultGeometryShader.h"
#include "Surface.h"
#include <memory>
#include <string>

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
        Output operator()(const Vertex &input) const;
        void set_diffuse_light(const Vec3f &c) { m_light_diff = c; }
        void set_ambient_light(const Vec3f &c) { m_light_amb = c; }
        void set_light_pos(const Vec3f &pos) { m_light_pos = pos; }
    private:
        Vec4f m_light_pos = Vec4f(0.f, 0.f, .5f, 1.f);
        Vec3f m_light_diff = Vec3f(1.f, 1.f, 1.f);
        Vec3f m_light_amb = Vec3f(.1f, .1f, .1f);
        float m_lin_att = 2.f;
        float m_quad_att = 2.619f;
        float m_const_att = .382f;
    };
    typedef DefaultGeometryShader<VertexShader::Output> GeometryShader;
    class PixelShader
    {
    public:
        template<typename I>
        Color operator()(const I &input) const;
        void bind_texture(const std::wstring &filename);
    private:
        std::unique_ptr<Surface> m_tex;
        unsigned int m_tex_width = 0u;
        unsigned int m_tex_height = 0u;
    };
public:
    VertexShader m_vs;
    GeometryShader m_gs;
    PixelShader m_ps;
};

template<typename I>
inline Color VertexLightTexEffect::PixelShader::operator()(const I &input) const
{
    const auto mat_color = Vec3f(m_tex->get_pixel(
        unsigned int(input.m_tc.m_x * m_tex_width + .5f) % m_tex_width,
        unsigned int(input.m_tc.m_y * m_tex_height + .5f) % m_tex_height
    )) / 255.f;
    return Color((mat_color * input.m_l).get_saturated() * 255.f);
}
