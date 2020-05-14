#pragma once
#include "ChiliWin.h"
#include "Colors.h"
#include "Rect.h"
#include "ChiliException.h"
#include <string>
#include <cassert>
#include <memory>

class Surface
{
public:
    class Exception : public ChiliException
    {
    public:
        using ChiliException::ChiliException;
        virtual std::wstring get_full_message() const override;
        virtual std::wstring get_exception_type() const override;
    };
public:
    Surface(unsigned int width, unsigned int height, unsigned int pitch);
    Surface(unsigned int width, unsigned int height) : Surface(width, height, width) { }
    Surface(Surface &&src) noexcept;
    Surface(Surface &) = delete;
    ~Surface() { }
    Surface &operator=(Surface &&src) noexcept;
    Surface &operator=(const Surface &) = delete;
    void clear(Color fill_value);
    void present(unsigned int dst_pitch, BYTE *const dst) const;
    void put_pixel(unsigned int x, unsigned int y, Color c);
    void put_pixel_alpha(unsigned int x, unsigned int y, Color c);
    Color get_pixel(unsigned int x, unsigned int y) const;
    unsigned int get_width() const;
    unsigned int get_height() const;
    unsigned int get_pitch() const;
    Color *get_buffer();
    const Color *get_buffer_const() const;
    void save(const std::wstring &filename) const;
    void copy(const Surface &src);
    static Surface from_file(const std::wstring &filename);
private:
    // calculate pixel pitch required for given byte alignment (must be multiple of 4 bytes)
    static unsigned int get_pitch(unsigned int width, unsigned int byte_alignment);
    Surface(unsigned int width, unsigned int height, unsigned int pitch,
        std::unique_ptr<Color[]> buffer);
private:
    std::unique_ptr<Color[]> m_buffer;
    unsigned int m_width;
    unsigned int m_height;
    unsigned int m_pitch;
};
