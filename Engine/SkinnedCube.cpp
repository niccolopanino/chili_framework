#include "SkinnedCube.h"
#include <stdexcept>

SkinnedCube::SkinnedCube(float size)
{
    const float side = size / 2.f;
    const auto convert_tex_coord = [](float u, float v)
    {
        return Vec2f((u + 1.f) / 3.f, v / 4.f);
    };
    m_vertices.emplace_back(-side, -side, -side);
    m_tcs.emplace_back(convert_tex_coord( 1.f, 0.f));
    m_vertices.emplace_back( side, -side, -side);
    m_tcs.emplace_back(convert_tex_coord( 0.f, 0.f));
    m_vertices.emplace_back(-side,  side, -side);
    m_tcs.emplace_back(convert_tex_coord( 1.f, 1.f));
    m_vertices.emplace_back( side,  side, -side);
    m_tcs.emplace_back(convert_tex_coord( 0.f, 1.f));
    m_vertices.emplace_back(-side, -side,  side);
    m_tcs.emplace_back(convert_tex_coord( 1.f, 3.f));
    m_vertices.emplace_back( side, -side,  side);
    m_tcs.emplace_back(convert_tex_coord( 0.f, 3.f));
    m_vertices.emplace_back(-side,  side,  side);
    m_tcs.emplace_back(convert_tex_coord( 1.f, 2.f));
    m_vertices.emplace_back( side,  side,  side);
    m_tcs.emplace_back(convert_tex_coord( 0.f, 2.f));
    m_vertices.emplace_back(-side, -side, -side);
    m_tcs.emplace_back(convert_tex_coord( 1.f, 4.f));
    m_vertices.emplace_back( side, -side, -side);
    m_tcs.emplace_back(convert_tex_coord( 0.f, 4.f));
    m_vertices.emplace_back(-side, -side, -side);
    m_tcs.emplace_back(convert_tex_coord( 2.f, 1.f));
    m_vertices.emplace_back(-side, -side,  side);
    m_tcs.emplace_back(convert_tex_coord( 2.f, 2.f));
    m_vertices.emplace_back( side, -side, -side);
    m_tcs.emplace_back(convert_tex_coord(-1.f, 1.f));
    m_vertices.emplace_back( side, -side,  side);
    m_tcs.emplace_back(convert_tex_coord(-1.f, 2.f));
}

IndexedLineList SkinnedCube::get_lines() const
{
    throw std::runtime_error("Incomplete function SkinnedCube::get_lines!");
}

IndexedTriangleList<Vec3f> SkinnedCube::get_triangles() const
{
    return IndexedTriangleList<Vec3f>(m_vertices,
        {
             0,  2,  1,     2,  3,  1,
             4,  8,  5,     5,  8,  9,
             2,  6,  3,     3,  6,  7,
             4,  5,  7,     4,  7,  6,
             2, 10, 11,     2, 11,  6,
            12,  3,  7,    12,  7, 13
        }
    );
}

IndexedTriangleList<TexVertex> SkinnedCube::get_textured_triangles() const
{
    std::vector<TexVertex> tvertices;
    tvertices.reserve(m_vertices.size());
    for (size_t i = 0; i < m_vertices.size(); i++)
        tvertices.emplace_back(m_vertices[i], m_tcs[i]);
    return IndexedTriangleList<TexVertex>(std::move(tvertices),
        {
             0,  2,  1,     2,  3,  1,
             4,  8,  5,     5,  8,  9,
             2,  6,  3,     3,  6,  7,
             4,  5,  7,     4,  7,  6,
             2, 10, 11,     2, 11,  6,
            12,  3,  7,    12,  7, 13
        }
    );
}
