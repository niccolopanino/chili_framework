#pragma once
#include "Vec2.h"
#include "Vec3.h"
#include "Vec4.h"
#include "Colors.h"
#include "Surface.h"
#include "BaseVertexShader.h"
#include "DefaultGeometryShader.h"
#include "BasePhongShader.h"
#include "ChiliMath.h"
#include <cmath>

template<typename DiffuseParams, typename SpecularParams>
class RippleVertexSpecPhongEffect
{
public:
    class Vertex
    {
    public:
        Vertex() = default;
        Vertex(const Vec3f &pos) : m_pos(pos) { }
        Vertex(const Vec3f &pos, const Vertex &src) : m_pos(pos), m_tc(src.m_tc) { }
        Vertex(const Vec3f &pos, const Vec2f &tc) : m_pos(pos), m_tc(tc) { }
    public:
        Vec3f m_pos;
        Vec2f m_tc;
    };
    class VSOut
    {
    public:
        VSOut() = default;
        VSOut(const Vec4f &pos) : m_pos(pos) { }
        VSOut(const Vec4f &pos, const VSOut &src);
        VSOut(const Vec4f &pos, const Vec3f &n, const Vec3f &world_pos, const Vec2f &tc);
        VSOut operator+(const VSOut &rhs) const;
        VSOut &operator+=(const VSOut &rhs) { return *this = *this + rhs; }
        VSOut operator-(const VSOut &rhs) const;
        VSOut &operator-=(const VSOut &rhs) { return *this = *this - rhs; }
        VSOut operator*(float rhs) const;
        VSOut &operator*=(float rhs) { return *this = *this * rhs; }
        VSOut operator/(float rhs) const;
        VSOut &operator/=(float rhs) { return *this = *this / rhs; }
    public:
        Vec4f m_pos;
        Vec3f m_n;
        Vec3f m_world_pos;
        Vec2f m_tc;
    };
    class VertexShader : public BaseVertexShader<VSOut>
    {
    public:
        void set_time(float time) { m_time = time; }
        typename BaseVertexShader<VSOut>::Output operator()(const Vertex &input) const
        {
            // calculate some triggy bois
            const auto angle = wrap_angle(input.m_pos.m_x * k_freq + m_time * k_wave_length);
            const auto cosx = std::cos(angle);
            const auto sinx = std::sin(angle);
            // sine wave amplitude from position with time variant phase animation
            const auto dz = k_amp * cosx;
            const Vec4f pos(input.m_pos.m_x, input.m_pos.m_y, input.m_pos.m_z + dz, 1.f);
            // normal derived base on cross product of partial dx cross dy
            Vec4f n(-k_freq * k_amp * sinx, 0.f, -1.f, 0.f);
            n.normalize();
            return Output(pos * m_world_view_proj, n * m_world_view,
                pos * m_world_view, input.m_tc);
        }
    private:
        static constexpr float k_wave_length = PI;
        static constexpr float k_freq = 45.f;
        static constexpr float k_amp = .02f;
        float m_time = 0.f;
    };
    typedef DefaultGeometryShader<typename VertexShader::Output> GeometryShader;
    class PixelShader : public BasePhongShader<DiffuseParams, SpecularParams>
    {
    public:
        template<typename I>
        Color operator()(const I &input) const;
        void bind_texture(const Surface &tex);
    private:
        const Surface *m_tex = nullptr;
        unsigned int m_tex_width = 0u;
        unsigned int m_tex_height = 0u;
    };
public:
    VertexShader m_vs;
    GeometryShader m_gs;
    PixelShader m_ps;
};

template<typename DiffuseParams, typename SpecularParams>
inline RippleVertexSpecPhongEffect<DiffuseParams, SpecularParams>::VSOut::VSOut(const Vec4f &pos,
    const VSOut &src)
    :
    m_pos(pos), m_n(src.m_n), m_world_pos(src.m_world_pos), m_tc(src.m_tc)
{ }

template<typename DiffuseParams, typename SpecularParams>
inline RippleVertexSpecPhongEffect<DiffuseParams, SpecularParams>::VSOut::VSOut(const Vec4f &pos,
    const Vec3f &n, const Vec3f &world_pos, const Vec2f &tc)
    :
    m_pos(pos), m_n(n), m_world_pos(world_pos), m_tc(tc)
{ }

template<typename DiffuseParams, typename SpecularParams>
inline typename RippleVertexSpecPhongEffect<DiffuseParams, SpecularParams>::VSOut
RippleVertexSpecPhongEffect<DiffuseParams, SpecularParams>::VSOut::operator+(const VSOut &rhs) const
{
    return VSOut(m_pos + rhs.m_pos, m_n + rhs.m_n, m_world_pos + rhs.m_world_pos, m_tc + rhs.m_tc);
}

template<typename DiffuseParams, typename SpecularParams>
inline typename RippleVertexSpecPhongEffect<DiffuseParams, SpecularParams>::VSOut
RippleVertexSpecPhongEffect<DiffuseParams, SpecularParams>::VSOut::operator-(const VSOut &rhs) const
{
    return VSOut(m_pos - rhs.m_pos, m_n - rhs.m_n, m_world_pos - rhs.m_world_pos, m_tc - rhs.m_tc);
}

template<typename DiffuseParams, typename SpecularParams>
inline typename RippleVertexSpecPhongEffect<DiffuseParams, SpecularParams>::VSOut
RippleVertexSpecPhongEffect<DiffuseParams, SpecularParams>::VSOut::operator*(float rhs) const
{
    return VSOut(m_pos * rhs, m_n * rhs, m_world_pos * rhs, m_tc * rhs);
}

template<typename DiffuseParams, typename SpecularParams>
inline typename RippleVertexSpecPhongEffect<DiffuseParams, SpecularParams>::VSOut
RippleVertexSpecPhongEffect<DiffuseParams, SpecularParams>::VSOut::operator/(float rhs) const
{
    return VSOut(m_pos / rhs, m_n / rhs, m_world_pos / rhs, m_tc / rhs);
}

template<typename DiffuseParams, typename SpecularParams>
template<typename I>
inline Color RippleVertexSpecPhongEffect<DiffuseParams, SpecularParams>::PixelShader::operator()(
    const I &input) const
{
    const Vec3f mat_color = Vec3f(m_tex->get_pixel(
        unsigned int(input.m_tc.m_x * m_tex_width + .5f) % m_tex_width,
        unsigned int(input.m_tc.m_y * m_tex_height + .5f) % m_tex_height
    )) / 255.f;
    return shade(input, mat_color);
}

template<typename DiffuseParams, typename SpecularParams>
inline void RippleVertexSpecPhongEffect<DiffuseParams, SpecularParams>::PixelShader::bind_texture(
    const Surface &tex)
{
    m_tex = &tex;
    m_tex_width = m_tex->get_width();
    m_tex_height = m_tex->get_height();
}
