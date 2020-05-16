#include "XQuads.h"

XQuads::XQuads(float size)
{
    const float side = size / 2.f;
    m_vertices.emplace_back(-side, -side, -side);
    m_vertices.emplace_back( side, -side, -side);
    m_vertices.emplace_back(-side,  side, -side);
    m_vertices.emplace_back( side,  side, -side);
    m_vertices.emplace_back(-side, -side,  side);
    m_vertices.emplace_back( side, -side,  side);
    m_vertices.emplace_back(-side,  side,  side);
    m_vertices.emplace_back( side,  side,  side);
}

IndexedLineList XQuads::get_lines() const
{
    return {
        m_vertices,
        {
            0, 4,   4, 7,   7, 3,   3, 0,
            1, 5,   5, 6,   6, 2,   2, 1
        }
    };
}

IndexedTriangleList XQuads::get_triangles() const
{
    return {
        m_vertices,
        {
            0, 4, 3,   4, 7, 3,
            2, 6, 1,   6, 5, 1
        }
    };
}
