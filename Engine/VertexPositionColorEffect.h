#pragma once
#include "Vec3.h"
#include "Mat.h"
#include "Colors.h"
#include "DefaultGeometryShader.h"

// color gradient effect between vertices determined by vertex position
class VertexPositionColorEffect
{
public:
    // the vertex type that will be input into the pipeline
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
    // uses x, y, z position components to determine color
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
        void bind_rotation(const Mat3f &rot) { m_rot = rot; }
        void bind_translation(const Vec3f &trans) { m_trans = trans; }
        Output operator()(const Vertex &input) const;
    private:
        Mat3f m_rot = Mat3f::identity();
        Vec3f m_trans;
    };
    // default geometry shader passes vertices through and outputs triangle
    typedef DefaultGeometryShader<VertexShader::Output> GeometryShader;
    // converts float color into packed byte color
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
