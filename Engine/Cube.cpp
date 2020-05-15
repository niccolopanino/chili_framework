#include "Cube.h"

Cube::Cube(float size)
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
