#pragma once
#include "Vec3.h"
#include "Vec4.h"
#include "Colors.h"
#include "DefaultVertexShader.h"
#include "DefaultGeometryShader.h"

class SolidEffect
{
public:
    // the vertex type that will be input into the pipeline
    class Vertex
    {
    public:
        Vertex() = default;
        Vertex(const Vec3f &pos) : m_pos(pos) { }
        Vertex(const Vec3f &pos, const Vertex &src) : m_pos(pos), m_color(src.m_color) { }
        Vertex(const Vec3f &pos, const Color &color) : m_pos(pos), m_color(color) { }
        Vertex operator+(const Vertex &rhs) const { return Vertex(m_pos + rhs.m_pos, m_color); }
        Vertex &operator+=(const Vertex &rhs) { return *this = *this + rhs; }
        Vertex operator-(const Vertex &rhs) const { return Vertex(m_pos - rhs.m_pos, m_color); }
        Vertex &operator-=(const Vertex &rhs) { return *this = *this - rhs; }
        Vertex operator*(float rhs) const { return Vertex(m_pos * rhs, m_color); }
        Vertex &operator*=(float rhs) { return *this = *this * rhs; }
        Vertex operator/(float rhs) const { return Vertex(m_pos / rhs, m_color); }
        Vertex &operator/=(float rhs) { return *this = *this / rhs; }
    public:
        Vec3f m_pos;
        Color m_color;
    };
    class VertexShader
    {
    public:
        class Output
        {
        public:
            Output() = default;
            Output(const Vec4f &pos) : m_pos(pos) { }
            Output(const Vec4f &pos, const Output &src) : m_pos(pos), m_color(src.m_color) { }
            Output(const Vec4f &pos, const Color &color) : m_pos(pos), m_color(color) { }
            Output operator+(const Output &rhs) const { return Output(m_pos + rhs.m_pos, m_color); }
            Output &operator+=(const Output &rhs) { return *this = *this + rhs; }
            Output operator-(const Output &rhs) const { return Output(m_pos - rhs.m_pos, m_color); }
            Output &operator-=(const Output &rhs) { return *this = *this - rhs; }
            Output operator*(float rhs) const { return Output(m_pos * rhs, m_color); }
            Output &operator*=(float rhs) { return *this = *this * rhs; }
            Output operator/(float rhs) const { return Output(m_pos / rhs, m_color); }
            Output &operator/=(float rhs) { return *this = *this / rhs; }
        public:
            Vec4f m_pos;
            Color m_color;
        };
    public:
        void bind_world_view(const Mat4f &world_view);
        void bind_projection(const Mat4f &proj);
        const Mat4f &get_proj() const { return m_proj; }
        Output operator()(const Vertex &input) const;
    private:
        Mat4f m_world_view = Mat4f::identity();
        Mat4f m_proj = Mat4f::identity();
        Mat4f m_world_view_proj = Mat4f::identity();
    };
    // default geometry shader passes vertices through and outputs triangle
    typedef DefaultGeometryShader<VertexShader::Output> GeometryShader;
    // invoked for each pixel of a triangle
    // takes an input of attributes that are the result of interpolating vertex attributes
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
