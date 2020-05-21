#pragma once
#include "Vec3.h"
#include "Mat.h"
#include "Colors.h"
#include "DefaultGeometryShader.h"
#include "ChiliMath.h"
#include <algorithm>

// per pixel lighting
class PhongPointEffect
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
    // duplicate pos to have one "clean" world position
    class VertexShader
    {
    public:
        class Output
        {
        public:
            Output() = default;
            Output(const Vec3f &pos) : m_pos(pos) { }
            Output(const Vec3f &pos, const Output &src);
            Output(const Vec3f &pos, const Vec3f &n, const Vec3f &world_pos);
            Output operator+(const Output &rhs) const;
            Output &operator+=(const Output &rhs) { return *this = *this + rhs; }
            Output operator-(const Output &rhs) const;
            Output &operator-=(const Output &rhs) { return *this = *this - rhs; }
            Output operator*(float rhs) const;
            Output &operator*=(float rhs) { return *this = *this * rhs; }
            Output operator/(float rhs) const;
            Output &operator/=(float rhs) { return *this = *this / rhs; }
        public:
            Vec3f m_pos;
            Vec3f m_n;
            Vec3f m_world_pos;
        };
    public:
        void bind_rotation(const Mat3f &rot) { m_rot = rot; }
        void bind_translation(const Vec3f &trans) { m_trans = trans; }
        Output operator()(const Vertex &input) const;
    private:
        Mat3f m_rot = Mat3f::identity();
        Vec3f m_trans;
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
    };
public:
    VertexShader m_vs;
    GeometryShader m_gs;
    PixelShader m_ps;
};

template<typename I>
inline Color PhongPointEffect::PixelShader::operator()(const I &input) const
{
    // vertex to light data
    const auto v_to_l = m_lpos - input.m_world_pos;
    const auto dist = v_to_l.len();
    const auto dir = v_to_l / dist;
    // calculate attenuation
    const auto att = 1.f / (m_const_att + m_lin_att * dist + m_quad_att * sq(dist));
    // calculate intensity based on angle of incidence and attenutation
    const auto d = m_diff * att * std::max(0.f, Vec3f::dot(input.m_n.get_normalized(), dir));
    // add diffuse, ambient, filter by material color, saturate and scale
    return Color((m_mat_color * (d + m_amb)).saturate() * 255.f);
}