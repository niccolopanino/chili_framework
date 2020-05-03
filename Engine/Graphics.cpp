/******************************************************************************************
*	Chili DirectX Framework Version 16.07.20											  *
*	Graphics.cpp																		  *
*	Copyright 2016 PlanetChili.net <http://www.planetchili.net>							  *
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
#include "MainWindow.h"
#include "Graphics.h"
#include "DXErr.h"
#include "ChiliException.h"
#include <assert.h>
#include <string>
#include <array>

// Ignore the intellisense error "cannot open source file" for .shh files.
// They will be created during the build sequence before the preprocessor runs.
namespace FramebufferShaders
{
#include "FramebufferPS.shh"
#include "FramebufferVS.shh"
}

#pragma comment( lib,"d3d11.lib" )

#define CHILI_GFX_EXCEPTION( hr,note ) Graphics::Exception( hr,note,_CRT_WIDE(__FILE__),__LINE__ )

using Microsoft::WRL::ComPtr;

IRect Graphics::get_screen_rect()
{
    return { 0, k_screen_width, 0, k_screen_height };
}

Graphics::Graphics(HWNDKey &key) : m_mapped_sysbuffer_texture({ 0 })
{
    assert(key.m_hwnd != nullptr);

    //////////////////////////////////////////////////////
    // create device and swap chain/get render target view
    DXGI_SWAP_CHAIN_DESC sd = {};
    sd.BufferCount = 1;
    sd.BufferDesc.Width = Graphics::k_screen_width;
    sd.BufferDesc.Height = Graphics::k_screen_height;
    sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 1;
    sd.BufferDesc.RefreshRate.Denominator = 60;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = key.m_hwnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;

    HRESULT				hr;
    UINT				createFlags = 0u;
#ifdef CHILI_USE_D3D_DEBUG_LAYER
#ifdef _DEBUG
    createFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
#endif

    // create device and front/back buffers
    if (FAILED(hr = D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        createFlags,
        nullptr,
        0,
        D3D11_SDK_VERSION,
        &sd,
        &m_swapchain_ptr,
        &m_device_ptr,
        nullptr,
        &m_device_context_ptr)))
    {
        throw CHILI_GFX_EXCEPTION(hr, L"Creating device and swap chain");
    }

    // get handle to backbuffer
    ComPtr<ID3D11Resource> pBackBuffer;
    if (FAILED(hr = m_swapchain_ptr->GetBuffer(
        0,
        __uuidof(ID3D11Texture2D),
        (LPVOID *)&pBackBuffer)))
    {
        throw CHILI_GFX_EXCEPTION(hr, L"Getting back buffer");
    }

    // create a view on backbuffer that we can render to
    if (FAILED(hr = m_device_ptr->CreateRenderTargetView(
        pBackBuffer.Get(),
        nullptr,
        &m_render_target_ptr)))
    {
        throw CHILI_GFX_EXCEPTION(hr, L"Creating render target view on backbuffer");
    }

    // set backbuffer as the render target using created view
    m_device_context_ptr->OMSetRenderTargets(1, m_render_target_ptr.GetAddressOf(), nullptr);

    // set viewport dimensions
    D3D11_VIEWPORT vp;
    vp.Width = float(Graphics::k_screen_width);
    vp.Height = float(Graphics::k_screen_height);
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0.0f;
    vp.TopLeftY = 0.0f;
    m_device_context_ptr->RSSetViewports(1, &vp);

    ///////////////////////////////////////
    // create texture for cpu render target
    D3D11_TEXTURE2D_DESC sysTexDesc;
    sysTexDesc.Width = Graphics::k_screen_width;
    sysTexDesc.Height = Graphics::k_screen_height;
    sysTexDesc.MipLevels = 1;
    sysTexDesc.ArraySize = 1;
    sysTexDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    sysTexDesc.SampleDesc.Count = 1;
    sysTexDesc.SampleDesc.Quality = 0;
    sysTexDesc.Usage = D3D11_USAGE_DYNAMIC;
    sysTexDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    sysTexDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    sysTexDesc.MiscFlags = 0;
    // create the texture
    if (FAILED(hr = m_device_ptr->CreateTexture2D(&sysTexDesc, nullptr, &m_sysbuffer_texture_ptr)))
    {
        throw CHILI_GFX_EXCEPTION(hr, L"Creating sysbuffer texture");
    }

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = sysTexDesc.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;
    // create the resource view on the texture
    if (FAILED(hr = m_device_ptr->CreateShaderResourceView(m_sysbuffer_texture_ptr.Get(),
        &srvDesc, &m_sysbuffer_texture_view_ptr)))
    {
        throw CHILI_GFX_EXCEPTION(hr, L"Creating view on sysBuffer texture");
    }

    ////////////////////////////////////////////////
    // create pixel shader for framebuffer
    // Ignore the intellisense error "namespace has no member"
    if (FAILED(hr = m_device_ptr->CreatePixelShader(
        FramebufferShaders::FramebufferPSBytecode,
        sizeof(FramebufferShaders::FramebufferPSBytecode),
        nullptr,
        &m_fshader_ptr)))
    {
        throw CHILI_GFX_EXCEPTION(hr, L"Creating pixel shader");
    }

    /////////////////////////////////////////////////
    // create vertex shader for framebuffer
    // Ignore the intellisense error "namespace has no member"
    if (FAILED(hr = m_device_ptr->CreateVertexShader(
        FramebufferShaders::FramebufferVSBytecode,
        sizeof(FramebufferShaders::FramebufferVSBytecode),
        nullptr,
        &m_vshader_ptr)))
    {
        throw CHILI_GFX_EXCEPTION(hr, L"Creating vertex shader");
    }

    //////////////////////////////////////////////////////////////
    // create and fill vertex buffer with quad for rendering frame
    const FSQVertex vertices[] =
    {
        { -1.0f,1.0f,0.5f,0.0f,0.0f },
        { 1.0f,1.0f,0.5f,1.0f,0.0f },
        { 1.0f,-1.0f,0.5f,1.0f,1.0f },
        { -1.0f,1.0f,0.5f,0.0f,0.0f },
        { 1.0f,-1.0f,0.5f,1.0f,1.0f },
        { -1.0f,-1.0f,0.5f,0.0f,1.0f },
    };
    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(FSQVertex) * 6;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0u;
    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = vertices;
    if (FAILED(hr = m_device_ptr->CreateBuffer(&bd, &initData, &m_vertex_buffer_ptr)))
    {
        throw CHILI_GFX_EXCEPTION(hr, L"Creating vertex buffer");
    }

    //////////////////////////////////////////
    // create input layout for fullscreen quad
    const D3D11_INPUT_ELEMENT_DESC ied[] =
    {
        { "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
        { "TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0 }
    };

    // Ignore the intellisense error "namespace has no member"
    if (FAILED(hr = m_device_ptr->CreateInputLayout(ied, 2,
        FramebufferShaders::FramebufferVSBytecode,
        sizeof(FramebufferShaders::FramebufferVSBytecode),
        &m_input_layout_ptr)))
    {
        throw CHILI_GFX_EXCEPTION(hr, L"Creating input layout");
    }

    ////////////////////////////////////////////////////
    // Create sampler state for fullscreen textured quad
    D3D11_SAMPLER_DESC sampDesc = {};
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    if (FAILED(hr = m_device_ptr->CreateSamplerState(&sampDesc, &m_sampler_state_ptr)))
    {
        throw CHILI_GFX_EXCEPTION(hr, L"Creating sampler state");
    }

    // allocate memory for sysbuffer (16-byte aligned for faster access)
    m_sysbuffer = reinterpret_cast<Color *>(
        _aligned_malloc(sizeof(Color) * Graphics::k_screen_width * Graphics::k_screen_height, 16u));
}

Graphics::~Graphics()
{
    // free sysbuffer memory (aligned free)
    if (m_sysbuffer)
    {
        _aligned_free(m_sysbuffer);
        m_sysbuffer = nullptr;
    }
    // clear the state of the device context before destruction
    if (m_device_context_ptr) m_device_context_ptr->ClearState();
}

void Graphics::end_frame()
{
    HRESULT hr;

    // lock and map the adapter memory for copying over the sysbuffer
    if (FAILED(hr = m_device_context_ptr->Map(m_sysbuffer_texture_ptr.Get(), 0u,
        D3D11_MAP_WRITE_DISCARD, 0u, &m_mapped_sysbuffer_texture)))
    {
        throw CHILI_GFX_EXCEPTION(hr, L"Mapping sysbuffer");
    }
    // setup parameters for copy operation
    Color *pDst = reinterpret_cast<Color *>(m_mapped_sysbuffer_texture.pData);
    const size_t dstPitch = m_mapped_sysbuffer_texture.RowPitch / sizeof(Color);
    const size_t srcPitch = Graphics::k_screen_width;
    const size_t rowBytes = srcPitch * sizeof(Color);
    // perform the copy line-by-line
    for (size_t y = 0u; y < Graphics::k_screen_height; y++)
    {
        memcpy(&pDst[y * dstPitch], &m_sysbuffer[y * srcPitch], rowBytes);
    }
    // release the adapter memory
    m_device_context_ptr->Unmap(m_sysbuffer_texture_ptr.Get(), 0u);

    // render offscreen scene texture to back buffer
    m_device_context_ptr->IASetInputLayout(m_input_layout_ptr.Get());
    m_device_context_ptr->VSSetShader(m_vshader_ptr.Get(), nullptr, 0u);
    m_device_context_ptr->PSSetShader(m_fshader_ptr.Get(), nullptr, 0u);
    m_device_context_ptr->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    const UINT stride = sizeof(FSQVertex);
    const UINT offset = 0u;
    m_device_context_ptr->IASetVertexBuffers(0u, 1u, m_vertex_buffer_ptr.GetAddressOf(), &stride, &offset);
    m_device_context_ptr->PSSetShaderResources(0u, 1u, m_sysbuffer_texture_view_ptr.GetAddressOf());
    m_device_context_ptr->PSSetSamplers(0u, 1u, m_sampler_state_ptr.GetAddressOf());
    m_device_context_ptr->Draw(6u, 0u);

    // flip back/front buffers
    if (FAILED(hr = m_swapchain_ptr->Present(1u, 0u)))
    {
        if (hr == DXGI_ERROR_DEVICE_REMOVED)
        {
            throw CHILI_GFX_EXCEPTION(m_device_ptr->GetDeviceRemovedReason(), L"Presenting back buffer [device removed]");
        } else
        {
            throw CHILI_GFX_EXCEPTION(hr, L"Presenting back buffer");
        }
    }
}

void Graphics::begin_frame()
{
    // clear the sysbuffer
    memset(m_sysbuffer, 0u, sizeof(Color) * Graphics::k_screen_height * Graphics::k_screen_width);
}

void Graphics::put_pixel(int x, int y, int r, int g, int b)
{
    put_pixel(x, y, { unsigned char(r),unsigned char(g),unsigned char(b) });
}

void Graphics::put_pixel(int x, int y, Color c)
{
    assert(x >= 0);
    assert(x < int(Graphics::k_screen_width));
    assert(y >= 0);
    assert(y < int(Graphics::k_screen_height));
    m_sysbuffer[Graphics::k_screen_width * y + x] = c;
}

void Graphics::draw_sprite_non_chroma(int x, int y, const Surface &s)
{
    draw_sprite_non_chroma(x, y, s.get_rect(), s);
}

void Graphics::draw_sprite_non_chroma(int x, int y, const IRect &src_rect, const Surface &s)
{
    draw_sprite_non_chroma(x, y, src_rect, get_screen_rect(), s);
}

void Graphics::draw_sprite_non_chroma(int x, int y, IRect src_rect, const IRect &clip, const Surface &s)
{
    assert(src_rect.m_left >= 0);
    assert(src_rect.m_right <= s.get_width());
    assert(src_rect.m_top >= 0);
    assert(src_rect.m_bottom <= s.get_height());

    if (x < clip.m_left) {
        src_rect.m_left += clip.m_left - x;
        x = clip.m_left;
    }
    if (y < clip.m_top) {
        src_rect.m_top += clip.m_top - y;
        y = clip.m_top;
    }
    if (x + src_rect.get_width() > clip.m_right)
        src_rect.m_right -= x + src_rect.get_width() - clip.m_right;
    if (y + src_rect.get_height() > clip.m_bottom)
        src_rect.m_bottom -= y + src_rect.get_height() - clip.m_bottom;

    for (int dy = src_rect.m_top; dy < src_rect.m_bottom; dy++) {
        for (int dx = src_rect.m_left; dx < src_rect.m_right; dx++)
            put_pixel(x + dx - src_rect.m_left, y + dy - src_rect.m_top, s.get_pixel(dx, dy));
    }
}

void Graphics::draw_sprite(int x, int y, const Surface &s, Color chroma)
{
    draw_sprite(x, y, s.get_rect(), s, chroma);
}

void Graphics::draw_sprite(int x, int y, const IRect &src_rect, const Surface &s, Color chroma)
{
    draw_sprite(x, y, src_rect, get_screen_rect(), s, chroma);
}

void Graphics::draw_sprite(int x, int y, IRect src_rect, const IRect &clip, const Surface &s, Color chroma)
{
    assert(src_rect.m_left >= 0);
    assert(src_rect.m_right <= s.get_width());
    assert(src_rect.m_top >= 0);
    assert(src_rect.m_bottom <= s.get_height());

    if (x < clip.m_left) {
        src_rect.m_left += clip.m_left - x;
        x = clip.m_left;
    }
    if (y < clip.m_top) {
        src_rect.m_top += clip.m_top - y;
        y = clip.m_top;
    }
    if (x + src_rect.get_width() > clip.m_right)
        src_rect.m_right -= x + src_rect.get_width() - clip.m_right;
    if (y + src_rect.get_height() > clip.m_bottom)
        src_rect.m_bottom -= y + src_rect.get_height() - clip.m_bottom;

    for (int dy = src_rect.m_top; dy < src_rect.m_bottom; dy++) {
        for (int dx = src_rect.m_left; dx < src_rect.m_right; dx++) {
            const Color src_pixel = s.get_pixel(dx, dy);
            if (src_pixel != chroma)
                put_pixel(x + dx - src_rect.m_left, y + dy - src_rect.m_top, src_pixel);
        }
    }
}

//////////////////////////////////////////////////
//           Graphics Exception
Graphics::Exception::Exception(HRESULT hr, const std::wstring &note, const wchar_t *file, unsigned int line)
    :
    ChiliException(file, line, note),
    m_hr(hr)
{ }

std::wstring Graphics::Exception::get_full_message() const
{
    const std::wstring empty = L"";
    const std::wstring errorName = get_error_name();
    const std::wstring errorDesc = get_error_description();
    const std::wstring &note = get_note();
    const std::wstring location = get_location();
    return    (!errorName.empty() ? std::wstring(L"Error: ") + errorName + L"\n"
        : empty)
        + (!errorDesc.empty() ? std::wstring(L"Description: ") + errorDesc + L"\n"
            : empty)
        + (!note.empty() ? std::wstring(L"Note: ") + note + L"\n"
            : empty)
        + (!location.empty() ? std::wstring(L"Location: ") + location
            : empty);
}

std::wstring Graphics::Exception::get_error_name() const
{
    return DXGetErrorString(m_hr);
}

std::wstring Graphics::Exception::get_error_description() const
{
    std::array<wchar_t, 512> wideDescription;
    DXGetErrorDescription(m_hr, wideDescription.data(), wideDescription.size());
    return wideDescription.data();
}

std::wstring Graphics::Exception::get_exception_type() const
{
    return L"Chili Graphics Exception";
}
