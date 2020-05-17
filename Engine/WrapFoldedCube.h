#pragma once
#include "Vec3.h"
#include <vector>
#include "IndexedLineList.h"
#include "IndexedTriangleList.h"
#include "TexVertex.h"

class WrapFoldedCube
{
public:
    WrapFoldedCube(float size);
    IndexedLineList get_lines() const;
    IndexedTriangleList<Vec3f> get_triangles() const;
    IndexedTriangleList<TexVertex> get_textured_triangles() const;
private:
    std::vector<Vec3f> m_vertices;
    std::vector<Vec2f> m_tcs;
};

