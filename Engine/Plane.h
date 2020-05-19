#pragma once
#include "IndexedTriangleList.h"
#include "Vec2.h"
#include "Vec3.h"
#include "ChiliMath.h"
#include <vector>
#include <array>

class Plane
{
public:
    template<typename V>
    static IndexedTriangleList<V> get_plain(int divisions = 7, float size = 1.f);
    template<typename V>
    static IndexedTriangleList<V> get_skinned(int divisions = 7, float size = 1.f);
};

template<typename V>
inline IndexedTriangleList<V> Plane::get_plain(int divisions, float size)
{
    const int num_vertices_side = divisions + 1;
    std::vector<V> vertices(sq(num_vertices_side + 1));
    // give V a constructor for pos only (improvements)
    {
        const float side = size / 2.f;
        const float division_size = size / float(divisions);
        const Vec3f bottom_left(-side, -side, 0.f);

        for (int y = 0, i = 0; y < num_vertices_side; y++) {
            const float ypos = float(y) * division_size;
            for (int x = 0; x < num_vertices_side; x++, i++)
                vertices[i].m_pos = bottom_left + Vec3f(float(x) * division_size, ypos, 0.f);
        }
    }

    std::vector<size_t> indices;
    indices.reserve((size_t)sq(divisions) * 6);
    {
        const auto vxy2i = [num_vertices_side](size_t x, size_t y)
        {
            return y * num_vertices_side + x;
        };
        for (size_t y = 0; y < divisions; y++) {
            for (size_t x = 0; x < divisions; x++) {
                const std::array<size_t, 4> index_array = {
                    vxy2i(x, y), vxy2i(x + 1, y), vxy2i(x, y + 1), vxy2i(x + 1, y + 1)
                };
                indices.push_back(index_array[0]);
                indices.push_back(index_array[2]);
                indices.push_back(index_array[1]);
                indices.push_back(index_array[1]);
                indices.push_back(index_array[2]);
                indices.push_back(index_array[3]);
            }
        }
    }

    return IndexedTriangleList<V>(std::move(vertices), std::move(indices));
}

template<typename V>
inline IndexedTriangleList<V> Plane::get_skinned(int divisions, float size)
{
    auto it_list = get_plain<V>(divisions, size);
    {
        const int num_vertices_side = divisions + 1;
        const float division_size = 1.f / float(divisions);
        const Vec2f bottom_left = Vec2f(0.f, 1.f);

        for (int y = 0, i = 0; y < num_vertices_side; y++) {
            const float ytex = -float(y) * division_size;
            for (int x = 0; x < num_vertices_side; x++, i++)
                it_list.m_vertices[i].m_tc = bottom_left + Vec2f(float(x) * division_size, ytex);
        }
    }
    return it_list;
}
