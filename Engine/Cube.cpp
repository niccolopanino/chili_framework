#include "Cube.h"

Cube::Cube(float size, float texdim)
{
    const float side = size / 2.f;
    m_vertices.emplace_back(-side, -side, -side);
    m_tcs.emplace_back(0.f, texdim);
    m_vertices.emplace_back( side, -side, -side);
    m_tcs.emplace_back(texdim, texdim);
    m_vertices.emplace_back(-side,  side, -side);
    m_tcs.emplace_back(0.f, 0.f);
    m_vertices.emplace_back( side,  side, -side);
    m_tcs.emplace_back(texdim, 0.f);
    m_vertices.emplace_back(-side, -side,  side);
    m_tcs.emplace_back(texdim, texdim);
    m_vertices.emplace_back( side, -side,  side);
    m_tcs.emplace_back(0.f, texdim);
    m_vertices.emplace_back(-side,  side,  side);
    m_tcs.emplace_back(texdim, 0.f);
    m_vertices.emplace_back( side,  side,  side);
    m_tcs.emplace_back(0.f, 0.f);
}

IndexedLineList Cube::get_lines() const
{
    return {
        m_vertices,
        {
            0, 1,   1, 3,   3, 2,   2, 0,
            0, 4,   1, 5,   3, 7,   2, 6,
            4, 5,   5, 7,   7, 6,   6, 4
        }
    };
}

IndexedTriangleList<Vec3f> Cube::get_triangles() const
{
    return IndexedTriangleList<Vec3f>(m_vertices,
        {
            0, 2, 1,   2, 3, 1,
            1, 3, 5,   3, 7, 5,
            2, 6, 3,   3, 6, 7,
            4, 5, 7,   4, 7, 6,
            0, 4, 2,   2, 4, 6,
            0, 1, 4,   1, 5, 4
        }
    );
}

IndexedTriangleList<TexVertex> Cube::get_textured_triangles() const
{
    std::vector<TexVertex> tvertices;
    tvertices.reserve(m_vertices.size());
    for (size_t i = 0; i < m_vertices.size(); i++)
        tvertices.emplace_back(m_vertices[i], m_tcs[i]);
    return IndexedTriangleList<TexVertex>(std::move(tvertices),
        {
            0, 2, 1,   2, 3, 1,
            1, 3, 5,   3, 7, 5,
            2, 6, 3,   3, 6, 7,
            4, 5, 7,   4, 7, 6,
            0, 4, 2,   2, 4, 6,
            0, 1, 4,   1, 5, 4
        }
    );
}
