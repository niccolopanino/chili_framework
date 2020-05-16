#pragma once
#include "Vec3.h"
#include <vector>
#include "IndexedLineList.h"
#include "IndexedTriangleList.h"

class Cube
{
public:
    Cube(float size);
    IndexedLineList get_lines() const;
    IndexedTriangleList get_triangles() const;
private:
    std::vector<Vec3f> m_vertices;
};
