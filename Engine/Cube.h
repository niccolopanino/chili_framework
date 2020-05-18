#pragma once
#include "Vec3.h"
#include <vector>
#include "IndexedTriangleList.h"

class Cube
{
public:
    template<typename V>
    static IndexedTriangleList<V> get_plain(float size = 1.f);
    template<typename V>
    static IndexedTriangleList<V> get_skinned(float size = 1.f);
};

template<typename V>
inline IndexedTriangleList<V> Cube::get_plain(float size)
{
    const float side = size / 2.f;

    std::vector<Vec3f> positions;
    positions.emplace_back(-side, -side, -side);
    positions.emplace_back(side, -side, -side);
    positions.emplace_back(-side, side, -side);
    positions.emplace_back(side, side, -side);
    positions.emplace_back(-side, -side, side);
    positions.emplace_back(side, -side, side);
    positions.emplace_back(-side, side, side);
    positions.emplace_back(side, side, side);

    std::vector<V> vertices(positions.size());
    for (size_t i = 0; i < positions.size(); i++) {
        vertices[i].m_pos = positions[i];
    }

    return IndexedTriangleList<V>(vertices,
        {
            0, 2, 1,
            2, 3, 1,
            1, 3, 5,
            3, 7, 5,
            2, 6, 3,
            3, 6, 7,
            4, 5, 7,
            4, 7, 6,
            0, 4, 2,
            2, 4, 6,
            0, 1, 4,
            1, 5, 4
        }
    );
}

template<typename V>
inline IndexedTriangleList<V> Cube::get_skinned(float size)
{
    const float side = size / 2.f;
    const auto convert_tex_coord = [](float u, float v)
    {
        return Vec2f((u + 1.f) / 3.f, v / 4.f);
    };

    std::vector<Vec3f> positions;
    std::vector<Vec2f> texcoords;

    positions.emplace_back(-side, -side, -side);
    texcoords.emplace_back(convert_tex_coord(1.f, 0.f));
    positions.emplace_back(side, -side, -side);
    texcoords.emplace_back(convert_tex_coord(0.f, 0.f));
    positions.emplace_back(-side, side, -side);
    texcoords.emplace_back(convert_tex_coord(1.f, 1.f));
    positions.emplace_back(side, side, -side);
    texcoords.emplace_back(convert_tex_coord(0.f, 1.f));
    positions.emplace_back(-side, -side, side);
    texcoords.emplace_back(convert_tex_coord(1.f, 3.f));
    positions.emplace_back(side, -side, side);
    texcoords.emplace_back(convert_tex_coord(0.f, 3.f));
    positions.emplace_back(-side, side, side);
    texcoords.emplace_back(convert_tex_coord(1.f, 2.f));
    positions.emplace_back(side, side, side);
    texcoords.emplace_back(convert_tex_coord(0.f, 2.f));
    positions.emplace_back(-side, -side, -side);
    texcoords.emplace_back(convert_tex_coord(1.f, 4.f));
    positions.emplace_back(side, -side, -side);
    texcoords.emplace_back(convert_tex_coord(0.f, 4.f));
    positions.emplace_back(-side, -side, -side);
    texcoords.emplace_back(convert_tex_coord(2.f, 1.f));
    positions.emplace_back(-side, -side, side);
    texcoords.emplace_back(convert_tex_coord(2.f, 2.f));
    positions.emplace_back(side, -side, -side);
    texcoords.emplace_back(convert_tex_coord(-1.f, 1.f));
    positions.emplace_back(side, -side, side);
    texcoords.emplace_back(convert_tex_coord(-1.f, 2.f));

    std::vector<V> vertices(positions.size());
    for (size_t i = 0; i < positions.size(); i++) {
        vertices[i].m_pos = positions[i];
        vertices[i].m_tc = texcoords[i];
    }

    return IndexedTriangleList<V>(vertices,
        {
            0, 2, 1,
            2, 3, 1,
            4, 8, 5,
            5, 8, 9,
            2, 6, 3,
            3, 6, 7,
            4, 5, 7,
            4, 7, 6,
            2, 10, 11,
            2, 11, 6,
            12, 3, 7,
            12, 7, 13
        }
    );
}
