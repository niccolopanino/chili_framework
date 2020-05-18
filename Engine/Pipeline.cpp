#include "Pipeline.h"
#include "ChiliMath.h"
#include <algorithm>

Pipeline::Vertex Pipeline::Vertex::operator+(const Vertex &rhs) const
{
    return Vertex(m_pos + rhs.m_pos, m_tc + rhs.m_tc);
}

Pipeline::Vertex Pipeline::Vertex::operator-(const Vertex &rhs) const
{
    return Vertex(m_pos - rhs.m_pos, m_tc - rhs.m_tc);
}

Pipeline::Vertex Pipeline::Vertex::operator*(float rhs) const
{
    return Vertex(m_pos * rhs, m_tc * rhs);
}

Pipeline::Vertex Pipeline::Vertex::operator/(float rhs) const
{
    return Vertex(m_pos / rhs, m_tc / rhs);
}

void Pipeline::draw(IndexedTriangleList<Vertex> &tri_list)
{
    process_vertices(tri_list.m_vertices, tri_list.m_indices);
}

void Pipeline::bind_rotation(const Mat3f &rot)
{
    m_rot = rot;
}

void Pipeline::bind_translation(const Vec3f &trans)
{
    m_trans = trans;
}

void Pipeline::bind_texture(const std::wstring &filename)
{
    m_tex = std::make_unique<Surface>(Surface::from_file(filename));
}

void Pipeline::process_vertices(const std::vector<Vertex> &vertices,
    const std::vector<size_t> &indices)
{
    // create vertex vector for vs output
    std::vector<Vertex> out_vertices;
    // transform vertices using matrix + vector
    for (const auto &v : vertices)
        out_vertices.emplace_back(v.m_pos * m_rot + m_trans, v.m_tc);
    // assemble triangles from stream of indices and vertices
    assemble_triangles(out_vertices, indices);
}

void Pipeline::assemble_triangles(const std::vector<Vertex> &vertices,
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

void Pipeline::process_triangle(const Vertex &v0, const Vertex &v1, const Vertex &v2)
{
    // generate triangle from 3 vertices using geometry shader
    // and send to post-processing
    post_process_triangle_vertices(Triangle<Vertex>{v0, v1, v2});
}

void Pipeline::post_process_triangle_vertices(Triangle<Vertex> &triangle)
{
    // perspective divide and screen transform for all 3 vertices
    m_pms.transform(triangle.v0.m_pos);
    m_pms.transform(triangle.v1.m_pos);
    m_pms.transform(triangle.v2.m_pos);
    // draw the triangle
    draw_triangle(triangle);
}

void Pipeline::draw_triangle(const Triangle<Vertex> &triangle)
{
    // using pointers so we can swap (for sorting purposes)
    const Vertex *pv0 = &triangle.v0;
    const Vertex *pv1 = &triangle.v1;
    const Vertex *pv2 = &triangle.v2;

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

void Pipeline::draw_flat_top_triangle(const Vertex &itp0, const Vertex &itp1, const Vertex &itp2)
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

void Pipeline::draw_flat_bottom_triangle(const Vertex &itp0, const Vertex &itp1, const Vertex &itp2)
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

void Pipeline::draw_flat_triangle(const Vertex &itp0, const Vertex &itp1, const Vertex &itp2,
    const Vertex &dv0, const Vertex &dv1, Vertex itp_edge1)
{
    // create edge interpolant for left edge (always v0)
    auto itp_edge0 = itp0;

    // calculate start and end scanlines (the scanline AFTER the last line drawn)
    const int ystart = (int)ceil(itp0.m_pos.m_y - .5f);
    const int yend = (int)ceil(itp2.m_pos.m_y - .5f);

    // do interpolant prestep
    itp_edge0 += dv0 * (float(ystart) + .5f - itp0.m_pos.m_y);
    itp_edge1 += dv1 * (float(ystart) + .5f - itp0.m_pos.m_y);

    // prepare clamping constants
    const float tex_width = float(m_tex->get_width());
    const float tex_height = float(m_tex->get_height());
    const float tex_clamp_x = tex_width - 1.f;
    const float tex_clamp_y = tex_height - 1.f;

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
            // perform texture lookup, clamp and write pixel
            m_gfx.put_pixel(x, y, m_tex->get_pixel(
                (unsigned int)std::min(iline.m_tc.m_x * tex_width + .5f, tex_clamp_x),
                (unsigned int)std::min(iline.m_tc.m_y * tex_height + .5f, tex_clamp_y)
            ));
        }
    }
}
