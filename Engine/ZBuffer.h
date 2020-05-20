#pragma once

class ZBuffer
{
public:
    ZBuffer(int width, int height);
    ZBuffer(const ZBuffer &) = delete;
    ~ZBuffer();
    ZBuffer &operator=(const ZBuffer &) = delete;
    void clear();
    float &at(int x, int y);
    const float &at(int x, int y) const;
    bool test_and_set(int x, int y, float depth);
    int get_width() const;
    int get_height() const;
private:
    int m_width;
    int m_height;
    float *m_buffer = nullptr;
};
