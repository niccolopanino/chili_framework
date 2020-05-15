#pragma once
#include <vector>
#include "Vec3.h"

struct IndexedLineList
{
    std::vector<Vec3f> m_vertices;
    std::vector<size_t> m_indices;
};
