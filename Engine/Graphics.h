/******************************************************************************************
*	Chili DirectX Framework Version 16.07.20											  *
*	Graphics.h																			  *
*	Copyright 2016 PlanetChili <http://www.planetchili.net>								  *
*																						  *
*	This file is part of The Chili DirectX Framework.									  *
*																						  *
*	The Chili DirectX Framework is free software: you can redistribute it and/or modify	  *
*	it under the terms of the GNU General Public License as published by				  *
*	the Free Software Foundation, either version 3 of the License, or					  *
*	(at your option) any later version.													  *
*																						  *
*	The Chili DirectX Framework is distributed in the hope that it will be useful,		  *
*	but WITHOUT ANY WARRANTY; without even the implied warranty of						  *
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the						  *
*	GNU General Public License for more details.										  *
*																						  *
*	You should have received a copy of the GNU General Public License					  *
*	along with The Chili DirectX Framework.  If not, see <http://www.gnu.org/licenses/>.  *
******************************************************************************************/
#pragma once
#include "ChiliWin.h"
#include <d3d11.h>
#include <wrl.h>
#include "ChiliException.h"
#include "Colors.h"
#include "Surface.h"
#include "Rect.h"
#include <cassert>

class Graphics
{
public:
    class Exception : public ChiliException
    {
    public:
        Exception(HRESULT hr, const std::wstring &note, const wchar_t *file, unsigned int line);
        std::wstring get_error_name() const;
        std::wstring get_error_description() const;
        virtual std::wstring get_full_message() const override;
        virtual std::wstring get_exception_type() const override;
    private:
        HRESULT m_hr;
    };
private:
    // vertex format for the framebuffer fullscreen textured quad
    struct FSQVertex
    {
        float x, y, z;      // position
        float u, v;         // texcoords
    };
public:
    static IRect get_screen_rect();
public:
    Graphics(class HWNDKey &key);
    Graphics(const Graphics &) = delete;
    Graphics &operator=(const Graphics &) = delete;
    void end_frame();
    void begin_frame();
    Color get_pixel(int x, int y) const;
    void put_pixel(int x, int y, int r, int g, int b);
    void put_pixel(int x, int y, Color c);
    template<typename T>
    void draw_sprite(int x, int  y, const Surface &s, T effect);
    template<typename T>
    void draw_sprite(int x, int y, const IRect &src_rect, const Surface &s, T effect);
    template<typename T>
    void draw_sprite(int x, int y, IRect src_rect, const IRect &clip, const Surface &s, T effect);
    ~Graphics();
public:
    static constexpr int k_screen_width = 800;
    static constexpr int k_screen_height = 600;
private:
    Microsoft::WRL::ComPtr<IDXGISwapChain>              m_swapchain_ptr;
    Microsoft::WRL::ComPtr<ID3D11Device>                m_device_ptr;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext>         m_device_context_ptr;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView>      m_render_target_ptr;
    Microsoft::WRL::ComPtr<ID3D11Texture2D>             m_sysbuffer_texture_ptr;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>    m_sysbuffer_texture_view_ptr;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>           m_fshader_ptr;
    Microsoft::WRL::ComPtr<ID3D11VertexShader>          m_vshader_ptr;
    Microsoft::WRL::ComPtr<ID3D11Buffer>                m_vertex_buffer_ptr;
    Microsoft::WRL::ComPtr<ID3D11InputLayout>           m_input_layout_ptr;
    Microsoft::WRL::ComPtr<ID3D11SamplerState>          m_sampler_state_ptr;
    D3D11_MAPPED_SUBRESOURCE                            m_mapped_sysbuffer_texture;
    Surface m_sysbuffer;
};

template<typename T>
inline void Graphics::draw_sprite(int x, int y, const Surface &s, T effect)
{
    draw_sprite(x, y, s.get_rect(), s, effect);
}

template<typename T>
inline void Graphics::draw_sprite(int x, int y, const IRect &src_rect, const Surface &s, T effect)
{
    draw_sprite(x, y, src_rect, get_screen_rect(), s, effect);
}

template<typename T>
inline void Graphics::draw_sprite(int x, int y, IRect src_rect, const IRect &clip, const Surface &s, T effect)
{
    assert(src_rect.m_left >= 0);
    assert(src_rect.m_right <= s.get_width());
    assert(src_rect.m_top >= 0);
    assert(src_rect.m_bottom <= s.get_height());

    if (x < clip.m_left) {
        src_rect.m_left += clip.m_left - x;
        x = clip.m_left;
    }
    if (x + src_rect.get_width() > clip.m_right)
        src_rect.m_right -= x + src_rect.get_width() - clip.m_right;
    if (y < clip.m_top) {
        src_rect.m_top += clip.m_top - y;
        y = clip.m_top;
    }
    if (y + src_rect.get_height() > clip.m_bottom)
        src_rect.m_bottom -= y + src_rect.get_height() - clip.m_bottom;

    for (int dy = src_rect.m_top; dy < src_rect.m_bottom; dy++) {
        for (int dx = src_rect.m_left; dx < src_rect.m_right; dx++)
            effect(x + dx - src_rect.m_left, y + dy - src_rect.m_top, s.get_pixel(dx, dy), *this);
    }
}
