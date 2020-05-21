#include "ZBuffer.h"
#include <limits>
#include <cassert>
#include <algorithm>

ZBuffer::ZBuffer(int width, int height) :
    m_width(width), m_height(height), m_buffer(new float[(size_t)width * height])
{ }

ZBuffer::~ZBuffer()
{
    delete[] m_buffer;
    m_buffer = nullptr;
}

void ZBuffer::clear()
{
    const int ndepths = m_width * m_height;
    for (int i = 0; i < ndepths; i++)
        m_buffer[i] = std::numeric_limits<float>::infinity();
}

float &ZBuffer::at(int x, int y)
{
    assert(x >= 0);
    assert(x < m_width);
    assert(y >= 0);
    assert(y < m_height);
    return m_buffer[y * m_width + x];
}

const float &ZBuffer::at(int x, int y) const
{
    return const_cast<ZBuffer *>(this)->at(x, y);
}

bool ZBuffer::test_and_set(int x, int y, float depth)
{
    float &depth_in_buffer = at(x, y);
    if (depth < depth_in_buffer) {
        depth_in_buffer = depth;
        return true;
    }
    return false;
}

int ZBuffer::get_width() const
{
    return m_width;
}

int ZBuffer::get_height() const
{
    return m_height;
}

auto ZBuffer::get_min_max() const
{
    return std::minmax_element(m_buffer, m_buffer + (size_t)m_width * m_height);
}
