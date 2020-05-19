#pragma once
#include "Vec3.h"
#include "Triangle.h"
#include "Colors.h"
#include "DefaultVertexShader.h"
#include <vector>

class SolidGeometryEffect
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
    // gs colors vertices using their index from a table
    // every two triangles are colored from the same entry
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
        void bind_colors(std::vector<Color> colors) { m_tri_colors = std::move(colors); }
    private:
        std::vector<Color> m_tri_colors;
    };
    //invoked for each pixel of a triangle
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
