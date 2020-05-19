#pragma once
#include "ChiliWin.h"
#include "Graphics.h"
#include "Triangle.h"
#include "IndexedTriangleList.h"
#include "PubeScreenTransformer.h"
#include "ZBuffer.h"
#include "Mat3.h"
#include "Vec3.h"
#include "ChiliMath.h"
#include <algorithm>
#include <vector>

// triangle drawing pipeline with programmable pixel shading stage
template<typename E>
class Pipeline
{
public:
    // vertex type used for geometry and throughout pipeline
    typedef typename E::Vertex Vertex;
    typedef typename E::VertexShader::Output VSOut;
public:
    Pipeline(Graphics &gfx);
    void draw(IndexedTriangleList<Vertex> &tri_list);
    void begin_frame();
private:
    // vertex processing function
    // transforms vertices using vertex shader
    // and then passes vertex & index lists to triangle assembler
    void process_vertices(const std::vector<Vertex> &vertices,
        const std::vector<size_t> &indices);
    // triangle assembly function
    // assembles indexed vertex stream into triangles and passes them to post process
    // culls (does not send) back faced triangles
    void assemble_triangles(const std::vector<VSOut> &vertices,
        const std::vector<size_t> &indices);
    // triangles processing function
    // takes 3 vertices to generate triangle
    // sends generated triangle to post-processing
    void process_triangle(const VSOut &v0, const VSOut &v1, const VSOut &v2);
    // vertex post-processing function
    // perform perspective and viewport transformations
    void post_process_triangle_vertices(Triangle<VSOut> &triangle);
    // === triangle rasterization functions ===
    // itp0, itp1, etc. stand for interpolants
    // (values which are interpolated across a triangle in screen space)
    //
    // entry point for triangle rasterization
    // sorts vertices, determines case, splits to flat triangles,
    // dispatches to flat triangle functions
    void draw_triangle(const Triangle<VSOut> &triangle);
    // does flat *TOP* triangle-specific calculations and calls draw_flat_triangle
    void draw_flat_top_triangle(const VSOut &itp0, const VSOut &itp1, const VSOut &itp2);
    // does flat *BOTTOM* triangle-specific calculations and calls draw_flat_triangle
    void draw_flat_bottom_triangle(const VSOut &itp0, const VSOut &itp1, const VSOut &itp2);
    // does post processing common to both flat top and flat bottom triangles
    // scan over triangle in screen space, interpolate attributes,
    // depth cull, invoke pixel shader and write pixel to screen
    void draw_flat_triangle(const VSOut &itp0, const VSOut &itp1, const VSOut &itp2,
        const VSOut &dv0, const VSOut &dv1, VSOut itp_edge1);
public:
    E m_effect;
private:
    Graphics &m_gfx;
    PubeScreenTransformer m_pms;
    ZBuffer m_zb;
    Mat3f m_rot = Mat3f::identity();
    Vec3f m_trans;
};

template<typename E>
inline Pipeline<E>::Pipeline(Graphics &gfx) :
    m_gfx(gfx), m_zb(Graphics::k_screen_width, Graphics::k_screen_height)
{ }

template<typename E>
inline void Pipeline<E>::draw(IndexedTriangleList<Vertex> &tri_list)
{
    process_vertices(tri_list.m_vertices, tri_list.m_indices);
}

template<typename E>
inline void Pipeline<E>::begin_frame()
{
    m_zb.clear();
}

template<typename E>
void Pipeline<E>::process_vertices(const std::vector<Vertex> &vertices,
    const std::vector<size_t> &indices)
{
    // create vertex vector for vertex shader output
    std::vector<VSOut> out_vertices(vertices.size());
    // transform vertices using matrix + vector
    std::transform(vertices.begin(), vertices.end(), out_vertices.begin(), m_effect.m_vs);
    // assemble triangles from stream of indices and vertices
    assemble_triangles(out_vertices, indices);
}

template<typename E>
void Pipeline<E>::assemble_triangles(const std::vector<VSOut> &vertices,
    const std::vector<size_t> &indices)
{
    // assemble triangles in the stream and process
    for (size_t i = 0, end = indices.size() / 3; i < end; i++) {
        // determine triangle vertices via indexing
        const auto &v0 = vertices[indices[i * 3]];
        const auto &v1 = vertices[indices[i * 3 + 1]];
        const auto &v2 = vertices[indices[i * 3 + 2]];
        // cull backfacing triangles with cross product shenanigans
        if (Vec3f::dot(Vec3f::cross((v1.m_pos - v0.m_pos), (v2.m_pos - v0.m_pos)),
            v0.m_pos) <= 0.f)
        {
            // process 3 vertices into a triangle
            process_triangle(v0, v1, v2);
        }
    }
}

template<typename E>
void Pipeline<E>::process_triangle(const VSOut &v0, const VSOut &v1, const VSOut &v2)
{
    // generate triangle from 3 vertices using geometry shader
    // and send to post-processing
    post_process_triangle_vertices(Triangle<Vertex>{v0, v1, v2});
}

template<typename E>
void Pipeline<E>::post_process_triangle_vertices(Triangle<VSOut> &triangle)
{
    // perspective divide and screen transform for all 3 vertices
    m_pms.transform(triangle.v0);
    m_pms.transform(triangle.v1);
    m_pms.transform(triangle.v2);
    // draw the triangle
    draw_triangle(triangle);
}

template<typename E>
void Pipeline<E>::draw_triangle(const Triangle<VSOut> &triangle)
{
    // using pointers so we can swap (for sorting purposes)
    const VSOut *pv0 = &triangle.v0;
    const VSOut *pv1 = &triangle.v1;
    const VSOut *pv2 = &triangle.v2;

    // sorting vertices by y
    if (pv1->m_pos.m_y < pv0->m_pos.m_y) std::swap(pv0, pv1);
    if (pv2->m_pos.m_y < pv1->m_pos.m_y) std::swap(pv1, pv2);
    if (pv1->m_pos.m_y < pv0->m_pos.m_y) std::swap(pv0, pv1);

    // natural flat top
    if (pv0->m_pos.m_y == pv1->m_pos.m_y)
    {
        // sorting top vertices by x
        if (pv1->m_pos.m_x < pv0->m_pos.m_x) std::swap(pv0, pv1);
        draw_flat_top_triangle(*pv0, *pv1, *pv2);
    }
    // natural flat bottom
    else if (pv1->m_pos.m_y == pv2->m_pos.m_y)
    {
        // sorting bottom vertices by x
        if (pv2->m_pos.m_x < pv1->m_pos.m_x) std::swap(pv1, pv2);
        draw_flat_bottom_triangle(*pv0, *pv1, *pv2);
    }
    // general triangle
    else
    {
        // find splitting vertex interpolant
        const float alpha_split = (pv1->m_pos.m_y - pv0->m_pos.m_y)
            / (pv2->m_pos.m_y - pv0->m_pos.m_y);
        const auto vi = interpolate(*pv0, *pv2, alpha_split);

        //major right
        if (pv1->m_pos.m_x < vi.m_pos.m_x)
        {
            draw_flat_bottom_triangle(*pv0, *pv1, vi);
            draw_flat_top_triangle(*pv1, vi, *pv2);
        }
        // major left
        else
        {
            draw_flat_bottom_triangle(*pv0, vi, *pv1);
            draw_flat_top_triangle(vi, *pv1, *pv2);
        }
    }
}

template<typename E>
void Pipeline<E>::draw_flat_top_triangle(const VSOut &itp0, const VSOut &itp1, const VSOut &itp2)
{
    // calculate dvertex / dy change in interpolant for every 1 change in y
    const float ydelta = itp2.m_pos.m_y - itp0.m_pos.m_y;
    const auto ditp0 = (itp2 - itp0) / ydelta;
    const auto ditp1 = (itp2 - itp1) / ydelta;

    // create right edge interpolatn
    auto itp_edge_1 = itp1;

    // call the flat triangle render routine
    draw_flat_triangle(itp0, itp1, itp2, ditp0, ditp1, itp_edge_1);
}

template<typename E>
void Pipeline<E>::draw_flat_bottom_triangle(const VSOut &itp0, const VSOut &itp1, const VSOut &itp2)
{
    // calculate dvertex / dy change in interpolant for every 1 change in y
    const float ydelta = itp2.m_pos.m_y - itp0.m_pos.m_y;
    const auto ditp0 = (itp1 - itp0) / ydelta;
    const auto ditp1 = (itp2 - itp0) / ydelta;

    // create right edge interpolant
    auto itp_edge_1 = itp0;

    // call the flat triangle render routine
    draw_flat_triangle(itp0, itp1, itp2, ditp0, ditp1, itp_edge_1);
}

template<typename E>
void Pipeline<E>::draw_flat_triangle(const VSOut &itp0, const VSOut &itp1, const VSOut &itp2,
    const VSOut &dv0, const VSOut &dv1, VSOut itp_edge1)
{
    // create edge interpolant for left edge (always v0)
    auto itp_edge0 = itp0;

    // calculate start and end scanlines (the scanline AFTER the last line drawn)
    const int ystart = (int)ceil(itp0.m_pos.m_y - .5f);
    const int yend = (int)ceil(itp2.m_pos.m_y - .5f);

    // do interpolant prestep
    itp_edge0 += dv0 * (float(ystart) + .5f - itp0.m_pos.m_y);
    itp_edge1 += dv1 * (float(ystart) + .5f - itp0.m_pos.m_y);

    for (int y = ystart; y < yend; y++, itp_edge0 += dv0, itp_edge1 += dv1)
    {
        // calculate start and end pixels (the pixel AFTER the last pixel drawn)
        const int xstart = (int)ceil(itp_edge0.m_pos.m_x - .5f);
        const int xend = (int)ceil(itp_edge1.m_pos.m_x - .5f);

        // create scanline interpolant startpoint
        // (some waster for interpolating x / y / z,
        // but makes life easier not having to split them off
        // and z will be needed in the future anyways...)
        auto iline = itp_edge0;

        // calculate delta scanline interpolant / dx
        const float dx = itp_edge1.m_pos.m_x - itp_edge0.m_pos.m_x;
        const auto diline = (itp_edge1 - iline) / dx;

        // prestep scanline interpolant
        iline += diline * (float(xstart) + .5f - itp_edge0.m_pos.m_x);

        for (int x = xstart; x < xend; x++, iline += diline) {
            // recover interpolated z from interpolated 1/z
            const float z = 1.f / iline.m_pos.m_z;
            // do z rejection / update of z buffer
            // skip shading step if z rejected (early z)
            if (m_zb.test_and_set(x, y, z)) {
                // recover interpolated attributes
                // (wasted effort in multiplying pos (x / y / z) here, but
                // not a huge deal, not worth the code complication to fix)
                const auto attr = iline * z;
                // invoke pixel shader with interpolated vertex attributes
                /// and use result to set the pixel color on the screen
                m_gfx.put_pixel(x, y, m_effect.m_ps(attr));
            }
        }
    }
}
