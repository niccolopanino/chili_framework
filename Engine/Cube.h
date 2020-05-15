#pragma once
#include "Vec3.h"
#include <vector>
#include "IndexedLineList.h"

class Cube
{
public:
    Cube(float size);
    IndexedLineList get_lines() const;
private:
    std::vector<Vec3f> m_vertices;
};
