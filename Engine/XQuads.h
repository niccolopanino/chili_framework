#pragma once
#include "Vec3.h"
#include <vector>
#include "IndexedLineList.h"
#include "IndexedTriangleList.h"

class XQuads
{
public:
    XQuads(float size);
    IndexedLineList get_lines() const;
    IndexedTriangleList<Vec3f> get_triangles() const;
private:
    std::vector<Vec3f> m_vertices;
};
