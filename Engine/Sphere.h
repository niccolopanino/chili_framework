#pragma once
#include "Vec3.h"
#include "Mat3.h"
#include "IndexedTriangleList.h"
#include "ChiliMath.h"
#include <vector>

class Sphere
{
public:
    template<typename V>
    static IndexedTriangleList<V> get_plain(float radius = 1.f,
        int lat_div = 12, int long_div = 24);
    template<typename V>
    static IndexedTriangleList<V> get_plain_normals(float radius = 1.f,
        int lat_div = 12, int long_div = 24);
};

template<typename V>
inline IndexedTriangleList<V> Sphere::get_plain(float radius, int lat_div, int long_div)
{
    const Vec3f base(0.f, 0.f, radius);
    const float lat_angle = PI / lat_div;
    const float long_angle = 2.f * PI / long_div;

    // generate the vertices
    std::vector<V> vertices;
    for (int ilat = 1; ilat < lat_div; ilat++) {
        const auto lat_base = base * Mat3f::rotate_x(lat_angle * ilat);
        for (int ilong = 0; ilong < long_div; ilong++) {
            vertices.emplace_back();
            vertices.back().m_pos = lat_base * Mat3f::rotate_z(long_angle * ilong);
        }
    }
    // add the cap vertices
    const auto inorth_pole = vertices.size();
    vertices.emplace_back();
    vertices.back().m_pos = base;
    const auto isouth_pole = vertices.size();
    vertices.emplace_back();
    vertices.back().m_pos = -base;

    const auto calc_idx = [lat_div, long_div](int ilat, int ilong)
    {
        return ilat * long_div + ilong;
    };
    std::vector<size_t> indices;
    for (int ilat = 0; ilat < lat_div - 2; ilat++) {
        for (int ilong = 0; ilong < long_div - 1; ilong++) {
            // "inner" indices
            indices.push_back(calc_idx(ilat, ilong));
            indices.push_back(calc_idx(ilat + 1, ilong));
            indices.push_back(calc_idx(ilat, ilong + 1));
            indices.push_back(calc_idx(ilat, ilong + 1));
            indices.push_back(calc_idx(ilat + 1, ilong));
            indices.push_back(calc_idx(ilat + 1, ilong + 1));
        }
        // wrap band
        indices.push_back(calc_idx(ilat, long_div - 1));
        indices.push_back(calc_idx(ilat + 1, long_div - 1));
        indices.push_back(calc_idx(ilat, 0));
        indices.push_back(calc_idx(ilat, 0));
        indices.push_back(calc_idx(ilat + 1, long_div - 1));
        indices.push_back(calc_idx(ilat + 1, 0));
    }

    // cap fans
    for (int ilong = 0; ilong < long_div - 1; ilong++) {
        // north
        indices.push_back(inorth_pole);
        indices.push_back(calc_idx(0, ilong));
        indices.push_back(calc_idx(0, ilong + 1));
        // south
        indices.push_back(calc_idx(lat_div - 2, ilong + 1));
        indices.push_back(calc_idx(lat_div - 2, ilong));
        indices.push_back(isouth_pole);
    }
    // wrap triangles
    // north
    indices.push_back(inorth_pole);
    indices.push_back(calc_idx(0, long_div - 1));
    indices.push_back(calc_idx(0, 0));
    // south
    indices.push_back(calc_idx(lat_div - 2, 0));
    indices.push_back(calc_idx(lat_div - 2, long_div - 1));
    indices.push_back(isouth_pole);

    return { std::move(vertices), std::move(indices) };
}

template<typename V>
inline IndexedTriangleList<V> Sphere::get_plain_normals(float radius, int lat_div, int long_div)
{
    auto sphere = get_plain<V>(radius, lat_div, long_div);
    for (auto &v : sphere.m_vertices)
        v.m_n = v.m_pos.get_normalized();
    return sphere;
}
