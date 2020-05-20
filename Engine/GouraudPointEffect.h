#pragma once
#include "Vec3.h"
#include "Mat3.h"
#include "Colors.h"
#include "DefaultGeometryShader.h"

class GouraudPointEffect
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
    // calculate color based on normal to light angle
    // no interpolation of color attribute
    class VertexShader
    {
    public:
        class Output
        {
        public:
            Output() = default;
            Output(const Vec3f &pos) : m_pos(pos) { }
            Output(const Vec3f &pos, const Output &src) : m_pos(pos), m_color(src.m_color) { }
            Output(const Vec3f &pos, const Vec3f &color) : m_pos(pos), m_color(color) { }
            Output operator+(const Output &rhs) const;
            Output &operator+=(const Output &rhs) { return *this = *this + rhs; }
            Output operator-(const Output &rhs) const;
            Output &operator-=(const Output &rhs) { return *this = *this - rhs; }
            Output operator*(float rhs) const { return Output(m_pos * rhs, m_color * rhs); }
            Output &operator*=(float rhs) { return *this = *this * rhs; }
            Output operator/(float rhs) const { return Output(m_pos / rhs, m_color / rhs); }
            Output &operator/=(float rhs) { return *this = *this / rhs; }
        public:
            Vec3f m_pos;
            Vec3f m_color;
        };
    public:
        void bind_rotation(const Mat3f rot) { m_rot = rot; }
        void bind_translation(const Vec3f trans) { m_trans = trans; }
        Output operator()(const Vertex &input) const;
        void set_diffuse_light(const Vec3f &c) { m_light_diff = c; }
        void set_ambient_light(const Vec3f &c) { m_light_amb = c; }
        void set_light_position(const Vec3f &pos) { m_light_pos = pos; }
        void set_mat_color(Color c) { m_mat_color = Vec3f(c); }
    private:
        Mat3f m_rot = Mat3f::identity();
        Vec3f m_trans;
        Vec3f m_light_pos = Vec3f(0.f, 0.f, .5f);
        Vec3f m_light_diff = Vec3f(1.f, 1.f, 1.f);
        Vec3f m_light_amb = Vec3f(.1f, .1f, .1f);
        Vec3f m_mat_color = Vec3f(.8f, .85f, 1.f);
        float m_lin_attenuation = 1.f;
        float m_quad_attenuation = 2.619f;
        float m_const_attenuation = .382f;
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
        Color operator()(const I &input) const { return Color(input.m_color); }
    };
public:
    VertexShader m_vs;
    GeometryShader m_gs;
    PixelShader m_ps;
};
