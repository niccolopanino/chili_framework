#pragma once
#include "Vec2.h"
#include "Vec3.h"
#include "Surface.h"
#include "Colors.h"
#include <algorithm>
#include <string>

// basic texture effect
class TextureEffect
{
public:
    // the vertex type that will be input into the pipeline
    class Vertex
    {
    public:
        Vertex() = default;
        Vertex(const Vec3f &pos) : m_pos(pos) { }
        Vertex(const Vec3f &pos, const Vertex &src) : m_pos(pos), m_tc(src.m_tc) { }
        Vertex(const Vec3f &pos, const Vec2f &tc) : m_pos(pos), m_tc(tc) { }
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
        Vec2f m_tc;
    };
    // invoked for each pixel of a triangle
    // takes an input of attributes that are the result of interpolating vertex attributes
    // and outputs a color
    class PixelShader
    {
    public:
        template<typename I>
        Color operator()(const I &input) const;
        void bind_texture(const std::wstring &filename);
    private:
        std::unique_ptr<Surface> m_tex;
        float m_tex_width;
        float m_tex_height;
        float m_tex_clamp_x;
        float m_tex_clamp_y;
    };
public:
    PixelShader m_ps;
};

template<typename I>
inline Color TextureEffect::PixelShader::operator()(const I &input) const
{
    return m_tex->get_pixel(
        (unsigned int)std::min(input.m_tc.m_x * m_tex_width + .5f, m_tex_clamp_x),
        (unsigned int)std::min(input.m_tc.m_y * m_tex_height + .5f, m_tex_clamp_y)
    );
}
