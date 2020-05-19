#pragma once
#include <cassert>
#include <vector>
#include <string>
#include <fstream>
#include <cctype>
#include <algorithm>
#include <stdexcept>
#include "tiny_obj_loader.h"
#include "Vec3.h"

template<typename T>
class IndexedTriangleList
{
public:
    IndexedTriangleList() = default;
    IndexedTriangleList(std::vector<T> vertices, std::vector<size_t> indices) :
        m_vertices(std::move(vertices)), m_indices(std::move(indices))
    {
        assert(m_vertices.size() > 2);
        assert(m_indices.size() % 3 == 0);
    }
    static IndexedTriangleList<T> load(const std::string &filename);
public:
    std::vector<T> m_vertices;
    std::vector<size_t> m_indices;
};

template<typename T>
inline IndexedTriangleList<T> IndexedTriangleList<T>::load(const std::string &filename)
{
    IndexedTriangleList<T> itl;

    // check first line of file to see if CCW winding comment exists
    bool is_ccw = false;
    {
        std::ifstream file(filename);
        std::string first_line;
        std::getline(file, first_line);
        std::transform(first_line.begin(), first_line.end(), first_line.begin(), std::tolower);
        if (first_line.find("ccw") != std::string::npos)
            is_ccw = true;
    }

    // these will be filled by obj loading function
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::string err;

    // load / parse the obj file
    const bool ret = tinyobj::LoadObj(&attrib, &shapes, nullptr, &err, filename.c_str());

    // check for errors
    if (!err.empty() && err.substr(0, 4) != "WARN")
        throw std::runtime_error(("LoadObj returned error: " + err + " File: " + filename).c_str());
    if (!ret)
        throw std::runtime_error(("LoadObj returned false! File: " + filename).c_str());
    if (shapes.size() == 0u)
        throw std::runtime_error(("LoadObj object file had no shapes! File: " + filename).c_str());

    // extract vertex data
    itl.m_vertices.reserve(attrib.vertices.size() / 3u);
    for (int i = 0; i < attrib.vertices.size(); i += 3) {
        itl.m_vertices.emplace_back(
            Vec3f(attrib.vertices[i], attrib.vertices[i + 1], attrib.vertices[i + 2])
        );
    }

    // extract index data
    const auto &mesh = shapes[0].mesh;
    itl.m_indices.reserve(mesh.indices.size());
    for (size_t f = 0; f < mesh.num_face_vertices.size(); f++)
    {
        if (mesh.num_face_vertices[f] != 3u) {
            std::stringstream ssr;
            ssr << "LoadObj error face #" << f << " has "
                << mesh.num_face_vertices[f] << " vertices";
            throw std::runtime_error(ssr.str().c_str());
        }
        for (size_t vn = 0; vn < 3u; vn++) {
            const auto idx = mesh.indices[f * 3u + vn];
            itl.m_indices.push_back(size_t(idx.vertex_index));
        }
        // reverse winding if file marked as CCW
        if (is_ccw)
            std::swap(itl.m_indices.back(), *std::prev(itl.m_indices.end(), 2));
    }

    return itl;
}
