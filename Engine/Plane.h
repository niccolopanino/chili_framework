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
    static IndexedTriangleList<V> get_plain(int divisions_x = 7, int divisions_y = 7,
        float width = 1.f, float height = 1.f);
    template<typename V>
    static IndexedTriangleList<V> get_skinned(int divisions_x = 7, int divisions_y = 7,
        float width = 1.f, float height = 1.f, float tscale = 1.f);
    template<typename V>
    static IndexedTriangleList<V> get_normals(int divisions_x = 7, int divisions_y = 7,
        float width = 1.f, float height = 1.f);
    template<typename V>
    static IndexedTriangleList<V> get_skinned_normals(int divisions_x = 7, int divisions_y = 7,
        float width = 1.f, float height = 1.f, float tscale = 1.f);
};

template<typename V>
inline IndexedTriangleList<V> Plane::get_plain(int divisions_x, int divisions_y,
    float width, float height)
{
    const int num_vertices_x = divisions_x + 1;
    const int num_vertices_y = divisions_y + 1;
    std::vector<V> vertices(num_vertices_x * num_vertices_y);
    // give V a constructor for pos only (improvements)
    {
        const float side_x = width / 2.f;
        const float side_y = height / 2.f;
        const float division_size_x = width / float(divisions_x);
        const float division_size_y = height / float(divisions_y);
        const Vec3f bottom_left(-side_x, -side_y, 0.f);

        for (int y = 0, i = 0; y < num_vertices_y; y++) {
            const float ypos = float(y) * division_size_y;
            for (int x = 0; x < num_vertices_x; x++, i++)
                vertices[i].m_pos = bottom_left + Vec3f(float(x) * division_size_x, ypos, 0.f);
        }
    }

    std::vector<size_t> indices;
    indices.reserve((size_t)divisions_x * divisions_y * 6);
    {
        const auto vxy2i = [num_vertices_x](size_t x, size_t y)
        {
            return y * num_vertices_x + x;
        };
        for (size_t y = 0; y < divisions_y; y++) {
            for (size_t x = 0; x < divisions_x; x++) {
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
inline IndexedTriangleList<V> Plane::get_skinned(int divisions_x, int divisions_y,
    float width, float height, float tscale)
{
    auto it_list = get_plain<V>(divisions_x, divisions_y, width, height);
    {
        const int num_vertices_x = divisions_x + 1;
        const int num_vertices_y = divisions_y + 1;
        const float tdivision_size_x = 1.f / float(divisions_x);
        const float tdivision_size_y = 1.f / float(divisions_y);
        const Vec2f tbottom_left = Vec2f(0.f, 1.f);

        for (int y = 0, i = 0; y < num_vertices_y; y++) {
            const float ytex = -float(y) * tdivision_size_y / tscale;
            for (int x = 0; x < num_vertices_x; x++, i++) {
                it_list.m_vertices[i].m_tc = tbottom_left
                    + Vec2f(float(x) * tdivision_size_x / tscale, ytex);
            }
        }
    }
    return it_list;
}

template<typename V>
inline IndexedTriangleList<V> Plane::get_normals(int divisions_x, int divisions_y,
    float width, float height)
{
    auto itl = get_plain<V>(divisions_x, divisions_y, width, height);
    for (auto &v : itl.m_vertices)
        v.m_n = Vec3f(0.f, 0.f, -1.f);
    return itl;
}

template<typename V>
inline IndexedTriangleList<V> Plane::get_skinned_normals(int divisions_x, int divisions_y,
    float width, float height, float tscale)
{
    auto itl = get_skinned<V>(divisions_x, divisions_y, width, height, tscale);
    for (auto &v : itl.m_vertices)
        v.m_n = Vec3f(0.f, 0.f, -1.f);
    return itl;
}
