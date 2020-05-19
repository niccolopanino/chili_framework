#pragma once
#include "Vec2.h"
#include "Vec3.h"
#include "Mat3.h"
#include "Surface.h"
#include "Colors.h"
#include "DefaultGeometryShader.h"
#include <string>
#include <memory>

class WaveVertexTextureEffect
{
public:
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
    // perturbes vertices in y axis in sin wave based on x position and time
    class VertexShader
    {
    public:
        typedef Vertex Output;
    public:
        void bind_rotation(const Mat3f &rot) { m_rot = rot; }
        void bind_translation(const Vec3f &trans) { m_trans = trans; }
        Output operator()(const Vertex &input) const;
        void set_time(float t) { m_time = t; }
    private:
        Mat3f m_rot = Mat3f::identity();
        Vec3f m_trans;
        float m_time = 0.f;
        float m_freq_wave = 10.f;
        float m_freq_scroll = 5.f;
        float m_amplitude = .05f;
    };
    // default geometry shader passes vertices through and outputs triangle
    typedef DefaultGeometryShader<VertexShader::Output> GeometryShader;
    // texture clamped pixel shader
    class PixelShader
    {
    public:
        template<typename I>
        Color operator()(const I &input) const;
        void bind_texture(const std::wstring &filename);
    private:
        std::unique_ptr<Surface> m_tex;
        float m_tex_width = 0.f;
        float m_tex_height = 0.f;
        float m_tex_clamp_x = 0.f;
        float m_tex_clamp_y = 0.f;
    };
public:
    VertexShader m_vs;
    GeometryShader m_gs;
    PixelShader m_ps;
};

template<typename I>
inline Color WaveVertexTextureEffect::PixelShader::operator()(const I &input) const
{
    return m_tex->get_pixel(
        (unsigned int)std::min(input.m_tc.m_x * m_tex_width + .5f, m_tex_clamp_x),
        (unsigned int)std::min(input.m_tc.m_y * m_tex_height + .5f, m_tex_clamp_y)
    );
}
