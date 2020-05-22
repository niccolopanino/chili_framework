#pragma once
#include "Vec3.h"
#include "Vec4.h"
#include "Mat.h"
#include "Colors.h"
#include "BaseVertexShader.h"
#include "DefaultGeometryShader.h"
#include "ChiliMath.h"
#include <cmath>
#include <algorithm>

class SpecularPointEffect
{
public:
    // the vertex type that will be input into the pipeline
    class Vertex
    {
    public:
        Vertex() = default;
        Vertex(const Vec3f &pos) : m_pos(pos) { }
        Vertex(const Vec3f &pos, const Vertex &src) : m_pos(pos), m_n(src.m_n) { }
        Vertex(const Vec3f &pos, const Vec3f &n) : m_pos(pos), m_n(n) { }
        Vertex operator+(const Vertex &rhs) const;
        Vertex &operator+=(const Vertex &rhs) { return *this = *this + rhs; }
        Vertex operator-(const Vertex &rhs) const;
        Vertex &operator-=(const Vertex &rhs) { return *this = *this - rhs; }
        Vertex operator*(float rhs) const { return Vertex(m_pos * rhs, m_n * rhs); }
        Vertex &operator*=(float rhs) { return *this = *this * rhs; }
        Vertex operator/(float rhs) const { return Vertex(m_pos / rhs, m_n / rhs); }
        Vertex &operator/=(float rhs) { return *this = *this / rhs; }
    public:
        Vec3f m_pos;
        Vec3f m_n;
    };
    class VSOut
    {
    public:
        VSOut() = default;
        VSOut(const Vec4f &pos) : m_pos(pos) { }
        VSOut(const Vec4f &pos, const VSOut &src);
        VSOut(const Vec4f &pos, const Vec3f &n, const Vec3f &world_pos);
        VSOut operator+(const VSOut &rhs) const;
        VSOut &operator+=(const VSOut &rhs) { return *this = *this + rhs; }
        VSOut operator-(const VSOut &rhs) const;
        VSOut &operator-=(const VSOut &rhs) { return *this = *this - rhs; }
        VSOut operator*(float rhs) const;
        VSOut &operator*=(float rhs) { return *this = *this * rhs; }
        VSOut operator/(float rhs) const;
        VSOut &operator/=(float rhs) { return *this = *this / rhs; }
    public:
        Vec4f m_pos;
        Vec3f m_n;
        Vec3f m_world_pos;
    };
    // duplicate pos to have one "clean" world position
    class VertexShader : public BaseVertexShader<VSOut>
    {
    public:
        Output operator()(const Vertex &input) const;
    };
    // default geometry shader passes vertices through and outputs triangle
    typedef DefaultGeometryShader<VertexShader::Output> GeometryShader;
    //invoked for each pixel of a triangle
    // takes an input of attributes that are the
    // result of interpolating vertex attributes
    // and outputs a color
    class PixelShader
    {
    public:
        template<typename I>
        Color operator()(const I &input) const;
        void set_diffuse_light(const Vec3f &c) { m_diff = c; }
        void set_ambient_light(const Vec3f &c) { m_amb = c; }
        void set_light_position(const Vec3f &pos) { m_lpos = pos; }
    private:
        Vec3f m_lpos = Vec3f(0.f, 0.f, .5f);
        Vec3f m_diff = Vec3f(1.f, 1.f, 1.f);
        Vec3f m_amb = Vec3f(.1f, .1f, .1f);
        Vec3f m_mat_color = Vec3f(.8f, .85f, 1.f);
        float m_lin_att = 1.f;
        float m_quad_att = 2.619f;
        float m_const_att = .382f;
        float m_spec_pow = 30.f;
        float m_spec_in = .6f;
    };
public:
    VertexShader m_vs;
    GeometryShader m_gs;
    PixelShader m_ps;
};

template<typename I>
inline Color SpecularPointEffect::PixelShader::operator()(const I &input) const
{
    // re-normalize interpolated surface normal
    const auto n = input.m_n.get_normalized();
    // vertex to light data
    const auto v_to_l = m_lpos - input.m_world_pos;
    const auto dist = v_to_l.len();
    const auto dir = v_to_l / dist;
    // calculate attenuation
    const auto att = 1.f / (m_const_att + m_lin_att * dist + m_quad_att * sq(dist));
    // calculate intensity based on angle of incidence and attenutation
    const auto d = m_diff * att * std::max(0.f, Vec3f::dot(n, dir));
    // reflected light vector
    const auto r = n * Vec3f::dot(v_to_l, n) * 2.f - v_to_l;
    // calculate specular intesity based on angle between viewing vector
    // and reflection vector, narrow with power function
    const auto s = m_diff * m_spec_in * std::pow(
        std::max(0.f, Vec3f::dot(-r.get_normalized(), input.m_world_pos.get_normalized())),
        m_spec_pow
    );
    // add diffuse, ambient, filter by material color, saturate and scale
    return Color((m_mat_color * (d + m_amb)).saturate() * 255.f);
}
