#pragma once
#include "Vec3.h"
#include "Mat.h"
#include "Triangle.h"
#include "Colors.h"
#include "DefaultVertexShader.h"

class GeometryFlatEffect
{
public:
    class Vertex
    {
    public:
        Vertex() = default;
        Vertex(const Vec3f &pos) : m_pos(pos) { }
        Vertex(const Vec3f &pos, const Vertex &src) : m_pos(pos) { }
        Vertex operator+(const Vertex &rhs) const { return Vertex(m_pos + rhs.m_pos); }
        Vertex &operator+=(const Vertex &rhs) { return *this = *this + rhs; }
        Vertex operator-(const Vertex &rhs) const { return Vertex(m_pos - rhs.m_pos); }
        Vertex &operator-=(const Vertex &rhs) { return *this = *this - rhs; }
        Vertex operator*(float rhs) const { return Vertex(m_pos * rhs); }
        Vertex &operator*=(float rhs) { return *this = *this * rhs; }
        Vertex operator/(float rhs) const { return Vertex(m_pos / rhs); }
        Vertex &operator/=(float rhs) { return *this = *this / rhs; }
    public:
        Vec3f m_pos;
    };
    // default vertex shader rotates and translates vertices
    // does not touch attributes
    typedef DefaultVertexShader<Vertex> VertexShader;
    // calculate color based on face normal calculated from
    // cross product of geometry -- no interpolation of color
    class GeometryShader
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
            Output &operator*=(float rhs) { return *this = *this * rhs; }
            Output operator/(float rhs) const { return Output(m_pos / rhs, m_color); }
            Output &operator/=(float rhs) { return *this = *this / rhs; }
        public:
            Vec3f m_pos;
            Color m_color;
        };
    public:
        Triangle<Output> operator()(
            const VertexShader::Output &input0,
            const VertexShader::Output &input1,
            const VertexShader::Output &input2,
            size_t tri_idx) const;
        void set_diffuse_light(const Vec3f &c) { m_diff = c; }
        void set_ambient_light(const Vec3f &c) { m_amb = c; }
        void set_light_direction(const Vec3f &dl);
        void set_mat_color(Color c) { m_color = Vec3f(c); }
    private:
        Mat3f m_rot = Mat3f::identity();
        Vec3f m_trans;
        Vec3f m_dir = Vec3f(0.f, 0.f, 1.f);
        // this is the intensity if direct light from source
        // color light so need values per color component
        Vec3f m_diff = Vec3f(1.f, 1.f, 1.f);
        // this is intensity of indirect light that bounces off other obj in scene
        // color light so need values per color component
        Vec3f m_amb = Vec3f(.1f, .1f, .1f);
        // color of material (how much light of each color is reflected)
        Vec3f m_color = Vec3f(.8f, .85f, 1.f);
    };
    // invoked for each pixel of a triangle
    // takes an input of attributes that are the
    // result of interpolating vertex attributes
    // and outputs a color
    class PixelShader
    {
    public:
        template<class I>
        Color operator()(const I &input) { return input.m_color; }
    };
public:
    VertexShader m_vs;
    GeometryShader m_gs;
    PixelShader m_ps;
};
