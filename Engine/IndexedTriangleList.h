#pragma once
#include <cassert>
#include <vector>
#include <string>
#include <fstream>
#include <cctype>
#include <algorithm>
#include <stdexcept>
#include "tiny_obj_loader.h"
#include "Miniball.h"
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
    static IndexedTriangleList<T> load_normals(const std::string &filename);
    void adjust_to_true_center();
    float get_radius() const;
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
    // attrib.vertices is a flat std::vector of floats corresponding
    // to vertex positions, laid out as xyzxyzxyz...
    // first preallocate required space in OUR std::vector
    itl.m_vertices.reserve(attrib.vertices.size() / 3u);
    // iterate over individual vertices, construct Vec3s in OUR vector
    for (size_t i = 0; i < attrib.vertices.size(); i += 3) {
        itl.m_vertices.emplace_back(
            Vec3f(attrib.vertices[i], attrib.vertices[i + 1], attrib.vertices[i + 2])
        );
    }

    // extract index data
    // obj file can contain multiple meshes, we assume just 1
    const auto &mesh = shapes[0].mesh;
    // mesh contains a std::vector of num_face_vertices (uchar)
    // and a flat std::vector of indices. if all faces are triangles
    // then for any face f, the first index of that face is [f * 3n]
    itl.m_indices.reserve(mesh.indices.size());
    for (size_t f = 0; f < mesh.num_face_vertices.size(); f++)
    {
        // make sure there are no non-triangle faces
        if (mesh.num_face_vertices[f] != 3u) {
            std::stringstream ssr;
            ssr << "LoadObj error face #" << f << " has "
                << mesh.num_face_vertices[f] << " vertices";
            throw std::runtime_error(ssr.str().c_str());
        }
        // load set of 3 indices for each face into OUR index std::vector
        for (size_t vn = 0; vn < 3u; vn++) {
            const auto idx = mesh.indices[f * 3u + vn];
            itl.m_indices.push_back(size_t(idx.vertex_index));
        }
        // reverse winding if file marked as CCW
        // swapping any two indices reverse the winding dir of triangle
        if (is_ccw)
            std::swap(itl.m_indices.back(), *std::prev(itl.m_indices.end(), 2));
    }

    return itl;
}

template<typename T>
inline IndexedTriangleList<T> IndexedTriangleList<T>::load_normals(const std::string &filename)
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
    // attrib.vertices is a flat std::vector of floats corresponding
    // to vertex positions, laid out as xyzxyzxyz...
    // first preallocate required space in OUR std::vector
    itl.m_vertices.reserve(attrib.vertices.size() / 3u);
    // iterate over individual vertices, construct Vec3s in OUR vector
    for (size_t i = 0; i < attrib.vertices.size(); i += 3) {
        itl.m_vertices.emplace_back(
            Vec3f(attrib.vertices[i], attrib.vertices[i + 1], attrib.vertices[i + 2])
        );
    }

    // extract index data
    // obj file can contain multiple meshes, we assume just 1
    const auto &mesh = shapes[0].mesh;
    // mesh contains a std::vector of num_face_vertices (uchar)
    // and a flat std::vector of indices. if all faces are triangles
    // then for any face f, the first index of that face is [f * 3n]
    itl.m_indices.reserve(mesh.indices.size());
    for (size_t f = 0; f < mesh.num_face_vertices.size(); f++)
    {
        // make sure there are no non-triangle faces
        if (mesh.num_face_vertices[f] != 3u) {
            std::stringstream ssr;
            ssr << "LoadObj error face #" << f << " has "
                << mesh.num_face_vertices[f] << " vertices";
            throw std::runtime_error(ssr.str().c_str());
        }
        // load set of 3 indices for each face into OUR index std::vector
        for (size_t vn = 0; vn < 3u; vn++) {
            const auto idx = mesh.indices[f * 3u + vn];
            itl.m_indices.push_back(size_t(idx.vertex_index));
            // write normals into the vertices
            itl.m_vertices[(size_t)idx.vertex_index].m_n = Vec3f(
                attrib.normals[(size_t)3 * idx.normal_index],
                attrib.normals[(size_t)3 * idx.normal_index + 1],
                attrib.normals[(size_t)3 * idx.normal_index + 2]
            );
        }
        // reverse winding if file marked as CCW
        // swapping any two indices reverse the winding dir of triangle
        if (is_ccw)
            std::swap(itl.m_indices.back(), *std::prev(itl.m_indices.end(), 2));
    }

    return itl;
}

template<typename T>
inline void IndexedTriangleList<T>::adjust_to_true_center()
{
    // used to enable miniball to access vertex pos info
    struct VertexAccessor
    {
        // iterator type for iterating over vertices
        typedef std::vector<T>::const_iterator Pit;
        // it type for iterating over components of vertex
        // (pointer is used to iterate over members of class here)
        typedef const float *Cit;
        // functor that miniball uses to get element iter based on vertex iter
        Cit operator()(Pit it) const { return &it->m_pos.m_x; }
    };

    // solve the minimum bounding sphere
    Miniball::Miniball<VertexAccessor> mb(3, m_vertices.begin(), m_vertices.end());
    // get center of min sphere
    // result is a pointer to float[3] (what a shitty fuckin interface)
    const auto pc = mb.center();
    const Vec3f center(*pc, *std::next(pc), *std::next(pc, 2));
    // adjust all vertices so that center of minimal shere is at (0, 0)
    for (auto &v : m_vertices)
        v.m_pos -= center;
}

template<typename T>
inline float IndexedTriangleList<T>::get_radius() const
{
    // find element with max distance from (0, 0); that is our radius
    return std::max_element(m_vertices.begin(), m_vertices.end(),
        [](const T &v0, const T &v1) { return v0.m_pos.len_sq() < v1.m_pos.len_sq(); }
    )->m_pos.len();
}
