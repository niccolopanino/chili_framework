#include "Surface.h"
#include "ChiliWin.h"
#include <cassert>
#include <fstream>
#include <algorithm>

Surface::Surface(const std::string &filename)
{
    std::ifstream file(filename, std::ios::binary);
    assert(file);

    BITMAPFILEHEADER bm_fheader;
    file.read(reinterpret_cast<char *>(&bm_fheader), sizeof(bm_fheader));

    BITMAPINFOHEADER bm_info_header;
    file.read(reinterpret_cast<char *>(&bm_info_header), sizeof(bm_info_header));

    assert(bm_info_header.biBitCount == 24 || bm_info_header.biBitCount == 32);
    assert(bm_info_header.biCompression == BI_RGB);

    m_width = bm_info_header.biWidth;

    int ystart;
    int yend;
    int dy;
    // test for reverse row order
    if (bm_info_header.biHeight < 0) {
        m_height = -bm_info_header.biHeight;
        ystart = 0;
        yend = m_height;
        dy = 1;
    } else {
        m_height = bm_info_header.biHeight;
        ystart = m_height - 1;
        yend = -1;
        dy = -1;
    }

    m_pixels.resize((size_t)m_width * m_height);

    file.seekg(bm_fheader.bfOffBits);
    const int padding24 = (4 - (m_width * 3) % 4) % 4;

    for (int y = ystart; y != yend; y += dy) {
        for (int x = 0; x < m_width; x++) {
            if (bm_info_header.biBitCount == 24)
                put_pixel(x, y, Color(file.get(), file.get(), file.get()));
            else if (bm_info_header.biBitCount == 32)
                put_pixel(x, y, Color(file.get(), file.get(), file.get(), file.get()));
        }
        if (bm_info_header.biBitCount == 24)
            file.seekg(padding24, std::ios::cur);
    }
}

Surface::Surface(int width, int height) :
    m_width(width), m_height(height),
    m_pixels((size_t)width * height)
{ }

int Surface::get_width() const
{
    return m_width;
}

int Surface::get_height() const
{
    return m_height;
}

IRect Surface::get_rect() const
{
    return { 0, m_width, 0, m_height };
}

const Color *Surface::data() const
{
    return m_pixels.data();
}
