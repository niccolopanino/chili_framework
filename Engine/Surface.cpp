#define FULL_WINTARD
#include "ChiliWin.h"
#include "Surface.h"
#include "ChiliException.h"
namespace Gdiplus
{
    using std::min;
    using std::max;
}
#include <gdiplus.h>
#include <Gdipluspixelformats.h>
#include <sstream>

#pragma comment(lib, "gdiplus.lib")

std::wstring Surface::Exception::get_full_message() const
{
    return get_note() + L"\nAt: " + get_location();
}

std::wstring Surface::Exception::get_exception_type() const
{
    return L"Surface Exception";
}

Surface::Surface(unsigned int width, unsigned int height, unsigned int pitch) :
    m_width(width), m_height(height), m_pitch(pitch),
    m_buffer(std::make_unique<Color[]>((size_t)pitch * height))
{ }

Surface::Surface(Surface && src) noexcept :
    m_width(src.m_width), m_height(src.m_height), m_pitch(src.m_pitch),
    m_buffer(std::move(src.m_buffer))
{ }

Surface &Surface::operator=(Surface && src) noexcept
{
    m_width = src.m_width;
    m_height = src.m_height;
    m_pitch = src.m_pitch;
    m_buffer = std::move(src.m_buffer);
    src.m_buffer = nullptr;
    return *this;
}

void Surface::clear(Color fill_value)
{
    memset(m_buffer.get(), fill_value.m_dword, (size_t)m_pitch * m_height * sizeof(Color));
}

void Surface::present(unsigned int dst_pitch, BYTE *const dst) const
{
    for (unsigned int y = 0; y < m_height; y++)
        memcpy(&dst[dst_pitch * y], &m_buffer[(size_t)m_pitch * y], sizeof(Color) * m_width);
}

void Surface::put_pixel(unsigned int x, unsigned int y, Color c)
{
    assert(x >= 0);
    assert(x < m_width);
    assert(y >= 0);
    assert(y < m_height);
    m_buffer[(size_t)y * m_pitch + x] = c;
}

void Surface::put_pixel_alpha(unsigned int x, unsigned int y, Color c)
{
    assert(x >= 0);
    assert(x < m_width);
    assert(y >= 0);
    assert(y < m_height);
    // load source pixel
    const Color d = get_pixel(x, y);
    // blend channels
    const unsigned char rblend = (c.get_r() * c.get_a() + d.get_r() * (255u - c.get_a())) / 256u;
    const unsigned char gblend = (c.get_g() * c.get_a() + d.get_g() * (255u - c.get_a())) / 256u;
    const unsigned char bblend = (c.get_b() * c.get_a() + d.get_b() * (255u - c.get_a())) / 256u;
    // pack channels back into pixel and fire pixel onto surface
    put_pixel(x, y, Color(rblend, gblend, bblend));
}

Color Surface::get_pixel(unsigned int x, unsigned int y) const
{
    assert(x >= 0);
    assert(x < m_width);
    assert(y >= 0);
    assert(y < m_height);
    return m_buffer[(size_t)y * m_pitch + x];
}

unsigned int Surface::get_width() const
{
    return m_width;
}

unsigned int Surface::get_height() const
{
    return m_height;
}

unsigned int Surface::get_pitch() const
{
    return m_pitch;
}

Color *Surface::get_buffer()
{
    return m_buffer.get();
}

const Color *Surface::get_buffer_const() const
{
    return m_buffer.get();
}

void Surface::save(const std::wstring &filename) const
{
    auto get_encoder_clsid = [&filename](const WCHAR *format, CLSID *clsid) -> void
    {
        UINT num = 0;   // number of image encoders
        UINT size = 0;  // size of the image encoder array in bytes

        Gdiplus::ImageCodecInfo *img_codec_info = nullptr;
        Gdiplus::GetImageEncodersSize(&num, &size);
        if (size == 0) {
            std::wstringstream wos;
            wos << L"Saving surface to [" << filename << L"]: failed to get encoder; size == 0.";
            throw Exception(_CRT_WIDE(__FILE__), __LINE__, wos.str());
        }

        img_codec_info = (Gdiplus::ImageCodecInfo *)(malloc(size));
        if (img_codec_info == nullptr) {
            std::wstringstream wos;
            wos << L"Saving surface to [" << filename
                << L"]: failed to get encoder; failed to allocate memory.";
            throw Exception(_CRT_WIDE(__FILE__), __LINE__, wos.str());
        }

        Gdiplus::GetImageEncoders(num, size, img_codec_info);

        for (UINT j = 0; j < num; j++) {
            if (wcscmp(img_codec_info[j].MimeType, format) == 0) {
                *clsid = img_codec_info[j].Clsid;
                free(img_codec_info);
                return;
            }
        }

        free(img_codec_info);
        std::wstringstream wos;
        wos << L"Saving surface to [" << filename
            << L"]: failed to get encoder; failed to find matching encoder.";
        throw Exception(_CRT_WIDE(__FILE__), __LINE__, wos.str());
    };

    CLSID bmpid;
    get_encoder_clsid(L"image/bmp", &bmpid);
    Gdiplus::Bitmap bitmap(m_width, m_height, m_pitch * sizeof(Color),
        PixelFormat32bppARGB, (BYTE *)m_buffer.get());
    if (bitmap.Save(filename.c_str(), &bmpid, nullptr) != Gdiplus::Status::Ok) {
        std::wstringstream wos;
        wos << L"Saving surface to [" << filename << L"]: failed to save.";
        throw Exception(_CRT_WIDE(__FILE__), __LINE__, wos.str());
    }
}

void Surface::copy(const Surface &src)
{
    assert(m_width == src.m_width);
    assert(m_height == src.m_height);
    if (m_pitch == src.m_pitch)
        memcpy(m_buffer.get(), src.m_buffer.get(), (size_t)m_pitch * m_height * sizeof(Color));
    else {
        for (unsigned int y = 0; y < m_height; y++) {
            memcpy(&m_buffer[(size_t)m_pitch * y], &src.m_buffer[(size_t)m_pitch * y],
                sizeof(Color) * m_width);
        }
    }
}

Surface Surface::from_file(const std::wstring &filename)
{
    unsigned int width = 0;
    unsigned int height = 0;
    unsigned int pitch = 0;
    std::unique_ptr<Color[]> buffer = nullptr;

    {
        Gdiplus::Bitmap bitmap(filename.c_str());
        if (bitmap.GetLastStatus() != Gdiplus::Status::Ok) {
            std::wstringstream wos;
            wos << L"Loading image[" << filename << L"]: failed to load.";
            throw Exception(_CRT_WIDE(__FILE__), __LINE__, wos.str());
        }

        pitch = width = bitmap.GetWidth();
        height = bitmap.GetHeight();
        buffer = std::make_unique<Color[]>((size_t)width * height);
        
        for (unsigned int y = 0; y < height; y++) {
            for (unsigned int x = 0; x < width; x++) {
                Gdiplus::Color c;
                bitmap.GetPixel(x, y, &c);
                buffer[(size_t)y * pitch + x] = c.GetValue();
            }
        }
    }

    return Surface(width, height, pitch, std::move(buffer));
}

unsigned int Surface::get_pitch(unsigned int width, unsigned int byte_alignment)
{
    assert(byte_alignment % 4 == 0);
    const unsigned int pixel_alignment = byte_alignment / sizeof(Color);
    return width + (pixel_alignment - width % pixel_alignment) % pixel_alignment;
}

Surface::Surface(unsigned int width, unsigned int height, unsigned int pitch,
    std::unique_ptr<Color[]> buffer)
    :
    m_width(width),
    m_height(height),
    m_pitch(pitch),
    m_buffer(std::move(buffer))
{ }
