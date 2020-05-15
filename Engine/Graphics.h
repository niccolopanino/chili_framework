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
#include "GDIPlusManager.h"
#include "ChiliException.h"
#include "Surface.h"
#include "Colors.h"
#include "Vec2.h"

#define CHILI_GFX_EXCEPTION(hr, note) Graphics::Exception(hr, note, _CRT_WIDE(__FILE__), __LINE__)

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
    Graphics(class HWNDKey &key);
    Graphics(const Graphics &) = delete;
    ~Graphics();
    Graphics &operator=(const Graphics &) = delete;
    void begin_frame();
    void end_frame();
    void draw_line(const Vec2f &v1, const Vec2f &v2, Color c);
    void draw_line(float x1, float y1, float x2, float y2, Color c);
    void put_pixel(int x, int y, int r, int g, int b);
    void put_pixel(int x, int y, Color c);
private:
    Microsoft::WRL::ComPtr<IDXGISwapChain>              m_swapchain;
    Microsoft::WRL::ComPtr<ID3D11Device>                m_device;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext>         m_device_context;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView>      m_render_target;
    Microsoft::WRL::ComPtr<ID3D11Texture2D>             m_sysbuffer_texture;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>    m_sysbuffer_texture_view;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>           m_fshader;
    Microsoft::WRL::ComPtr<ID3D11VertexShader>          m_vshader;
    Microsoft::WRL::ComPtr<ID3D11Buffer>                m_vertex_buffer;
    Microsoft::WRL::ComPtr<ID3D11InputLayout>           m_input_layout;
    Microsoft::WRL::ComPtr<ID3D11SamplerState>          m_sampler_state;
    D3D11_MAPPED_SUBRESOURCE                            m_mapped_sysbuffer_texture;
    GDIPlusManager                                      m_gdip_man;
    Surface                                             m_sysbuffer;
public:
    static constexpr unsigned int k_screen_width = 640u;
    static constexpr unsigned int k_screen_height = 640u;
};
