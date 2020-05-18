#pragma once
#include "ChiliWin.h"
#include "Graphics.h"
#include "Surface.h"
#include "Triangle.h"
#include "IndexedTriangleList.h"
#include "PubeScreenTransformer.h"
#include "Mat3.h"
#include <string>

// fixed-function triangle drawing pipeline
// draws textured triangle lists with clamping
class Pipeline
{
public:
    // vertex type used for geometry and throughout pipeline
    class Vertex
    {
    public:
        Vertex() = default;
        Vertex(const Vec3f &pos) : m_pos(pos) { }
        // this enables template functions clone a vertex while changing the pos only
        Vertex(const Vec3f &pos, const Vertex &src) : m_pos(pos), m_tc(src.m_tc) { }
        Vertex(const Vec3f &pos, const Vec2f &tc) : m_pos(pos), m_tc(tc) { }
        Vertex operator+(const Vertex &rhs) const;
        Vertex &operator+=(const Vertex &rhs) { return *this = *this + rhs; }
        Vertex operator-(const Vertex &rhs) const;
        Vertex &operator-=(const Vertex &rhs) { return *this = *this - rhs; }
        Vertex operator*(float rhs) const;
        Vertex &operator*=(float rhs) { return *this = *this * rhs; }
        Vertex operator/(float rhs) const;
        Vertex &operator/=(float rhs) { return *this = *this / rhs; }
    public:
        Vec3f m_pos;
        Vec2f m_tc;
    };
public:
    Pipeline(Graphics &gfx) : m_gfx(gfx) { }
    void draw(IndexedTriangleList<Vertex> &tri_list);
    void bind_rotation(const Mat3f &rot);
    void bind_translation(const Vec3f &trans);
    void bind_texture(const std::wstring &filename);
private:
    // vertex processing function
    // transforms vertices and then passes vtx & idx lists to triangle assembler
    void process_vertices(const std::vector<Vertex> &vertices,
        const std::vector<size_t> &indices);
    // triangle assembly function
    // assembles indexed vertex stream into triangles and passes them to post process
    // culls (does not send) back faced triangles
    void assemble_triangles(const std::vector<Vertex> &vertices,
        const std::vector<size_t> &indices);
    // triangles processing function
    // takes 3 vertices to generate triangle
    // sends generated triangle to post-processing
    void process_triangle(const Vertex &v0, const Vertex &v1, const Vertex &v2);
    // vertex post-processing function
    // perform perspective and viewport transformations
    void post_process_triangle_vertices(Triangle<Vertex> &triangle);
    // === triangle rasterization functions ===
    // itp0, itp1, etc. stand for interpolants
    // (values which are interpolated across a triangle in screen space)
    //
    // entry point for triangle rasterization
    // sorts vertices, determines case, splits to flat triangles,
    // dispatches to flat triangle functions
    void draw_triangle(const Triangle<Vertex> &triangle);
    // does flat *TOP* triangle-specific calculations and calls draw_flat_triangle
    void draw_flat_top_triangle(const Vertex &itp0, const Vertex &itp1, const Vertex &itp2);
    // does flat *BOTTOM* triangle-specific calculations and calls draw_flat_triangle
    void draw_flat_bottom_triangle(const Vertex &itp0, const Vertex &itp1, const Vertex &itp2);
    // does post processing common to both flat top and flat bottom triangles
    // texture lookup and pixel written here
    void draw_flat_triangle(const Vertex &itp0, const Vertex &itp1, const Vertex &itp2,
        const Vertex &dv0, const Vertex &dv1, Vertex itp_edge1);
private:
    Graphics &m_gfx;
    PubeScreenTransformer m_pms;
    Mat3f m_rot = Mat3f::identity();
    Vec3f m_trans;
    std::unique_ptr<Surface> m_tex;
};
