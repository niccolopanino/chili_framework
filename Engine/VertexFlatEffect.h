#pragma once
#include "Vec3.h"
#include "Mat3.h"
#include "Colors.h"
#include "DefaultGeometryShader.h"

class VertexFlatEffect
{
public:
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
    class VertexShader
    {
    public:
        class Output
        {
        public:
            Output() = default;
            Output(const Vec3f &pos) : m_pos(pos) { }
            Output(const Vec3f &pos, const Output &src) : m_pos(pos), m_color(src.m_color) { }
            Output(const Vec3f &pos, const Color &color) : m_pos(pos), m_color(color) { }
            Output operator+(const Output &rhs) const { return Output(m_pos + rhs.m_pos, m_color); }
            Output &operator+=(const Output &rhs) { return *this = *this + rhs; }
            Output operator-(const Output &rhs) const { return Output(m_pos - rhs.m_pos, m_color); }
            Output &operator-=(const Output &rhs) { return *this = *this - rhs; }
            Output operator*(float rhs) const { return Output(m_pos * rhs, m_color); }
            Output operator*=(float rhs) { return *this = *this * rhs; }
            Output operator/(float rhs) const { return Output(m_pos / rhs, m_color); }
            Output operator/=(float rhs) { return *this = *this / rhs; }
        public:
            Vec3f m_pos;
            Color m_color;
        };
    public:
        void bind_rotation(const Mat3f &rot) { m_rot = rot; }
        void bind_translation(const Vec3f &trans) { m_trans = trans; }
        Output operator()(const Vertex &input) const;
        void set_diffuse_light(const Vec3f &c) { m_diff = c; }
        void set_ambient_light(const Vec3f &c) { m_amb = c; }
        void set_light_direction(const Vec3f &dl);
        void set_mat_color(Color c) { m_color = Vec3f(c); }
    private:
        Mat3f m_rot;
        Vec3f m_trans;
        Vec3f m_dir = Vec3f(0.f, 0.f, 1.f);
        Vec3f m_diff = Vec3f(1.f, 1.f, 1.f);
        Vec3f m_amb = Vec3f(.1f, .1f, .1f);
        Vec3f m_color = Vec3f(.8f, .85f, 1.f);
    };
    // default geometry shader passes vertices through and outputs triangle
    typedef DefaultGeometryShader<VertexShader::Output> GeometryShader;
    // inkoed for each pixel of a triangle
    // takes an input of attributes that are the
    // result of interpolating vertex attributes
    // and outputs a color
    class PixelShader
    {
    public:
        template<typename I>
        Color operator()(const I &input) const { return input.m_color; }
    };
public:
    VertexShader m_vs;
    GeometryShader m_gs;
    PixelShader m_ps;
};
