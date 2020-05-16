#pragma once
#include <cassert>
#include <vector>
#include "Vec3.h"

struct IndexedTriangleList
{
    IndexedTriangleList(std::vector<Vec3f> vertices, std::vector<size_t> indices) :
        m_vertices(std::move(vertices)), m_indices(std::move(indices))
    {
        assert(m_vertices.size() > 2);
        assert(m_indices.size() % 3 == 0);
        m_cull_flags.resize(m_indices.size() / 3, false);
    }
    std::vector<Vec3f> m_vertices;
    std::vector<size_t> m_indices;
    std::vector<bool> m_cull_flags;
};
