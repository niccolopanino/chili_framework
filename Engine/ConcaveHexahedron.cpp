#include "ConcaveHexahedron.h"

ConcaveHexahedron::ConcaveHexahedron(float size)
{
    const float side = size / 2.f;
    m_vertices.emplace_back( 0.f,   side,  0.f);
    m_vertices.emplace_back(-side, -side,  0.f);
    m_vertices.emplace_back( side, -side,  0.f);
    m_vertices.emplace_back( 0.f,   0.f,   side);
    m_vertices.emplace_back( 0.f,   0.f,  -side);
}

IndexedLineList ConcaveHexahedron::get_lines() const
{
    return {
        m_vertices,
        {
            0, 1,   1, 3,   3, 2,
            2, 0,   3, 0,   3, 4,
            1, 4,   2, 4,   4, 0
        }
    };
}

IndexedTriangleList ConcaveHexahedron::get_triangles() const
{
    return IndexedTriangleList(m_vertices,
        {
            1, 0, 3,   3, 0, 2,
            1, 4, 0,   4, 2, 0,
            3, 4, 1,   4, 3, 2
        }
    );
}
