#pragma once
#include "Vec3.h"
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
        Vertex operator+(const Vertex &rhs) const;
        Vertex &operator+=(const Vertex &rhs) { return *this = *this + rhs; }
        Vertex operator-(const Vertex &rhs) const;
        Vertex &operator-=(const Vertex &rhs) { return *this = *this - rhs; }
        Vertex operator*(float rhs) const;
        Vertex &operator*=(float rhs) { return *this = *this * rhs; }
        Vertex operator/(float rhs) const;
        Vertex &operator/=(float rhs) { return *this = *this / rhs; }
    public:
        Vec3f m_pos;
        Color m_color;
    };
    // default vertex shader rotates and translates vertices
    // does not touch attributes
    typedef DefaultVertexShader<Vertex> VertexShader;
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
