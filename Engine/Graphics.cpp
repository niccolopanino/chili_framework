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

using Microsoft::WRL::ComPtr;

Graphics::Graphics(HWNDKey &key) :
    m_mapped_sysbuffer_texture({ 0 }), m_sysbuffer(k_screen_width, k_screen_height)
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

    D3D_FEATURE_LEVEL   feature_levels_requested = D3D_FEATURE_LEVEL_9_1;
    UINT                num_levels_requested = 1;
    D3D_FEATURE_LEVEL   feature_levels_supported;
    HRESULT             hr;
    UINT                createFlags = 0u;
#ifdef _DEBUG
#ifdef USE_DIRECT3D_DEBUG_RUNTIME
    createFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
#endif

    // create device and front/back buffers
    if (FAILED(hr = D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        createFlags,
        &feature_levels_requested,
        num_levels_requested,
        D3D11_SDK_VERSION,
        &sd,
        &m_swapchain,
        &m_device,
        &feature_levels_supported,
        &m_device_context)))
    {
        throw CHILI_GFX_EXCEPTION(hr, L"Creating device and swap chain");
    }

    // get handle to backbuffer
    ComPtr<ID3D11Resource> pBackBuffer;
    if (FAILED(hr = m_swapchain->GetBuffer(
        0,
        __uuidof(ID3D11Texture2D),
        (LPVOID *)&pBackBuffer)))
    {
        throw CHILI_GFX_EXCEPTION(hr, L"Getting back buffer");
    }

    // create a view on backbuffer that we can render to
    if (FAILED(hr = m_device->CreateRenderTargetView(
        pBackBuffer.Get(),
        nullptr,
        &m_render_target)))
    {
        throw CHILI_GFX_EXCEPTION(hr, L"Creating render target view on backbuffer");
    }

    // set backbuffer as the render target using created view
    m_device_context->OMSetRenderTargets(1, m_render_target.GetAddressOf(), nullptr);

    // set viewport dimensions
    D3D11_VIEWPORT vp;
    vp.Width = float(Graphics::k_screen_width);
    vp.Height = float(Graphics::k_screen_height);
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0.0f;
    vp.TopLeftY = 0.0f;
    m_device_context->RSSetViewports(1, &vp);

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
    if (FAILED(hr = m_device->CreateTexture2D(&sysTexDesc, nullptr, &m_sysbuffer_texture)))
    {
        throw CHILI_GFX_EXCEPTION(hr, L"Creating sysbuffer texture");
    }

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = sysTexDesc.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;
    // create the resource view on the texture
    if (FAILED(hr = m_device->CreateShaderResourceView(m_sysbuffer_texture.Get(),
        &srvDesc, &m_sysbuffer_texture_view)))
    {
        throw CHILI_GFX_EXCEPTION(hr, L"Creating view on sysBuffer texture");
    }

    ////////////////////////////////////////////////
    // create pixel shader for framebuffer
    // Ignore the intellisense error "namespace has no member"
    if (FAILED(hr = m_device->CreatePixelShader(
        FramebufferShaders::FramebufferPSBytecode,
        sizeof(FramebufferShaders::FramebufferPSBytecode),
        nullptr,
        &m_fshader)))
    {
        throw CHILI_GFX_EXCEPTION(hr, L"Creating pixel shader");
    }

    /////////////////////////////////////////////////
    // create vertex shader for framebuffer
    // Ignore the intellisense error "namespace has no member"
    if (FAILED(hr = m_device->CreateVertexShader(
        FramebufferShaders::FramebufferVSBytecode,
        sizeof(FramebufferShaders::FramebufferVSBytecode),
        nullptr,
        &m_vshader)))
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
    if (FAILED(hr = m_device->CreateBuffer(&bd, &initData, &m_vertex_buffer)))
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
    if (FAILED(hr = m_device->CreateInputLayout(ied, 2,
        FramebufferShaders::FramebufferVSBytecode,
        sizeof(FramebufferShaders::FramebufferVSBytecode),
        &m_input_layout)))
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
    if (FAILED(hr = m_device->CreateSamplerState(&sampDesc, &m_sampler_state)))
    {
        throw CHILI_GFX_EXCEPTION(hr, L"Creating sampler state");
    }
}

Graphics::~Graphics()
{
    // clear the state of the device context before destruction
    if (m_device_context) m_device_context->ClearState();
}

void Graphics::begin_frame()
{
    // clear the sysbuffer
    m_sysbuffer.clear(Colors::Red);
}

void Graphics::end_frame()
{
    HRESULT hr;

    // lock and map the adapter memory for copying over the sysbuffer
    if (FAILED(hr = m_device_context->Map(m_sysbuffer_texture.Get(), 0u,
        D3D11_MAP_WRITE_DISCARD, 0u, &m_mapped_sysbuffer_texture)))
    {
        throw CHILI_GFX_EXCEPTION(hr, L"Mapping sysbuffer");
    }
    // perform the copy line-by-line
    m_sysbuffer.present(m_mapped_sysbuffer_texture.RowPitch,
        reinterpret_cast<BYTE *>(m_mapped_sysbuffer_texture.pData));
    // release the adapter memory
    m_device_context->Unmap(m_sysbuffer_texture.Get(), 0u);

    // render offscreen scene texture to back buffer
    m_device_context->IASetInputLayout(m_input_layout.Get());
    m_device_context->VSSetShader(m_vshader.Get(), nullptr, 0u);
    m_device_context->PSSetShader(m_fshader.Get(), nullptr, 0u);
    m_device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    const UINT stride = sizeof(FSQVertex);
    const UINT offset = 0u;
    m_device_context->IASetVertexBuffers(0u, 1u, m_vertex_buffer.GetAddressOf(), &stride, &offset);
    m_device_context->PSSetShaderResources(0u, 1u, m_sysbuffer_texture_view.GetAddressOf());
    m_device_context->PSSetSamplers(0u, 1u, m_sampler_state.GetAddressOf());
    m_device_context->Draw(6u, 0u);

    // flip back/front buffers
    if (FAILED(hr = m_swapchain->Present(1u, 0u)))
    {
        if (hr == DXGI_ERROR_DEVICE_REMOVED) {
            throw CHILI_GFX_EXCEPTION(m_device->GetDeviceRemovedReason(),
                L"Presenting back buffer [device removed]");
        } else {
            throw CHILI_GFX_EXCEPTION(hr, L"Presenting back buffer");
        }
    }
}

void Graphics::put_pixel(int x, int y, int r, int g, int b)
{
    put_pixel(x, y, { unsigned char(r),unsigned char(g),unsigned char(b) });
}

void Graphics::put_pixel(int x, int y, Color c)
{
    m_sysbuffer.put_pixel(x, y, c);
}

void Graphics::draw_line(const Vec2f &v1, const Vec2f &v2, Color c)
{
    draw_line(v1.m_x, v1.m_y, v2.m_x, v2.m_y, c);
}

void Graphics::draw_line(float x1, float y1, float x2, float y2, Color c)
{
    const float dx = x2 - x1;
    const float dy = y2 - y1;

    if (dy == 0.f && dx == 0.f)
        put_pixel(int(x1), int(y1), c);
    else if (abs(dy) > abs(dx))
    {
        if (dy < 0.f) {
            std::swap(x1, x2);
            std::swap(y1, y2);
        }

        const float m = dx / dy;
        float y = y1;
        int last_int_y = 0;
        for (float x = x1; y < y2; y += 1.f, x += m) {
            last_int_y = int(y);
            put_pixel(int(x), last_int_y, c);
        }
        if (int(y2) > last_int_y)
            put_pixel(int(x2), int(y2), c);
    } else
    {
        if (dx < 0.f) {
            std::swap(x1, x2);
            std::swap(y1, y2);
        }

        const float m = dy / dx;
        float x = x1;
        int last_int_x = 0;
        for (float y = y1; x < x2; x += 1.f, y += m) {
            last_int_x = int(x);
            put_pixel(last_int_x, int(y), c);
        }
        if (int(x2) > last_int_x)
            put_pixel(int(x2), int(y2), c);
    }
}

void Graphics::draw_triangle(const Vec2f &v1, const Vec2f &v2, const Vec2f &v3, Color c)
{
    // using pointers so we can swap (for sorting purposes)
    const Vec2f *pv1 = &v1;
    const Vec2f *pv2 = &v2;
    const Vec2f *pv3 = &v3;

    // sort vertices by y
    if (pv2->m_y < pv1->m_y) std::swap(pv1, pv2);
    if (pv3->m_y < pv2->m_y) std::swap(pv2, pv3);
    if (pv2->m_y < pv1->m_y) std::swap(pv1, pv2);

    // natural flat top
    if (pv1->m_y == pv2->m_y)
    {
        // sort top vertices by x
        if (pv2->m_x < pv1->m_x) std::swap(pv1, pv2);
        draw_flat_top_triangle(*pv1, *pv2, *pv3, c);
    }
    // natural flat bottom
    else if (pv2->m_y == pv3->m_y)
    {
        // sort bottom vertices by x
        if (pv3->m_x < pv2->m_x) std::swap(pv2, pv3);
        draw_flat_bottom_triangle(*pv1, *pv2, *pv3, c);
    }
    // general triangle
    else
    {
        // find splitting vertex
        const float alpha_split = (pv2->m_y - pv1->m_y) / (pv3->m_y - pv1->m_y);
        const Vec2f vi = *pv1 + (*pv3 - *pv1) * alpha_split;
        if (pv2->m_x < vi.m_x) { // major right
            draw_flat_bottom_triangle(*pv1, *pv2, vi, c);
            draw_flat_top_triangle(*pv2, vi, *pv3, c);
        } else { // major left
            draw_flat_bottom_triangle(*pv1, vi, *pv2, c);
            draw_flat_top_triangle(vi, *pv2, *pv3, c);
        }
    }
}

void Graphics::draw_textured_triangle(const TexVertex &v1, const TexVertex &v2,
    const TexVertex &v3, const Surface &tex)
{
    // using pointers so we can swap (for sorting purposes)
    const TexVertex *pv1 = &v1;
    const TexVertex *pv2 = &v2;
    const TexVertex *pv3 = &v3;

    // sort vertices by y
    if (pv2->m_pos.m_y < pv1->m_pos.m_y) std::swap(pv1, pv2);
    if (pv3->m_pos.m_y < pv2->m_pos.m_y) std::swap(pv2, pv3);
    if (pv2->m_pos.m_y < pv1->m_pos.m_y) std::swap(pv1, pv2);

    // natural flat top
    if (pv1->m_pos.m_y == pv2->m_pos.m_y)
    {
        // sort top vertices by x
        if (pv2->m_pos.m_x < pv1->m_pos.m_x) std::swap(pv1, pv2);
        draw_textured_flat_top_triangle(*pv1, *pv2, *pv3, tex);
    }
    // natural flat bottom
    else if (pv2->m_pos.m_y == pv3->m_pos.m_y)
    {
        // sort bottom vertices by x
        if (pv3->m_pos.m_x < pv2->m_pos.m_x) std::swap(pv2, pv3);
        draw_textured_flat_bottom_triangle(*pv1, *pv2, *pv3, tex);
    }
    // general triangle
    else
    {
        // find splitting vertex
        const float alpha_split = (pv2->m_pos.m_y - pv1->m_pos.m_y)
            / (pv3->m_pos.m_y - pv1->m_pos.m_y);
        const TexVertex vi = pv1->interpolate(*pv3, alpha_split);
        if (pv2->m_pos.m_x < vi.m_pos.m_x) { // major right
            draw_textured_flat_bottom_triangle(*pv1, *pv2, vi, tex);
            draw_textured_flat_top_triangle(*pv2, vi, *pv3, tex);
        } else { // major left
            draw_textured_flat_bottom_triangle(*pv1, vi, *pv2, tex);
            draw_textured_flat_top_triangle(vi, *pv2, *pv3, tex);
        }
    }
}

void Graphics::draw_flat_top_triangle(const Vec2f &v1, const Vec2f &v2, const Vec2f &v3, Color c)
{
    // calculate slopes in screen space
    const float m1 = (v3.m_x - v1.m_x) / (v3.m_y - v1.m_y);
    const float m2 = (v3.m_x - v2.m_x) / (v3.m_y - v2.m_y);

    // calculate start and end scanlines
    const int ystart = (int)ceil(v1.m_y - .5f);
    const int yend = (int)ceil(v3.m_y - .5f);

    for (int y = ystart; y < yend; y++)
    {
        // calculate start and end points (x-coords)
        // add 0.5 to y value because we're calculating based on pixel CENTERS
        const float px1 = m1 * (float(y) + .5f - v1.m_y) + v1.m_x;
        const float px2 = m2 * (float(y) + .5f - v2.m_y) + v2.m_x;

        // calculate start and end pixels
        const int xstart = (int)ceil(px1 - .5f);
        const int xend = (int)ceil(px2 - .5f); // the pixel AFTER the last pixel drawn

        for (int x = xstart; x < xend; x++)
            put_pixel(x, y, c);
    }
}

void Graphics::draw_flat_bottom_triangle(const Vec2f &v1, const Vec2f &v2, const Vec2f &v3, Color c)
{
    // calculate slopes in screen space
    const float m1 = (v2.m_x - v1.m_x) / (v2.m_y - v1.m_y);
    const float m2 = (v3.m_x - v1.m_x) / (v3.m_y - v1.m_y);

    // calculate start and end scanlines
    const int ystart = (int)ceil(v1.m_y - .5f);
    const int yend = (int)ceil(v3.m_y - .5f);

    for (int y = ystart; y < yend; y++)
    {
        // calculate start and end points (x-coords)
        // add 0.5 to y value because we're calculating based on pixel CENTERS
        const float px1 = m1 * (float(y) + .5f - v1.m_y) + v1.m_x;
        const float px2 = m2 * (float(y) + .5f - v1.m_y) + v1.m_x;

        // calculate start and end pixels
        const int xstart = (int)ceil(px1 - .5f);
        const int xend = (int)ceil(px2 - .5f); // the pixel AFTER the last pixel drawn

        for (int x = xstart; x < xend; x++)
            put_pixel(x, y, c);
    }
}

void Graphics::draw_textured_flat_top_triangle(const TexVertex &v1, const TexVertex &v2,
    const TexVertex &v3, const Surface &tex)
{
    // calculate slopes in screen space
    const float m1 = (v3.m_pos.m_x - v1.m_pos.m_x) / (v3.m_pos.m_y - v1.m_pos.m_y);
    const float m2 = (v3.m_pos.m_x - v2.m_pos.m_x) / (v3.m_pos.m_y - v2.m_pos.m_y);

    // calculate start and end scanlines
    const int ystart = (int)ceil(v1.m_pos.m_y - .5f);
    const int yend = (int)ceil(v3.m_pos.m_y - .5f);

    // init tex coord edges
    Vec2f tc_edge_l = v1.m_tc;
    Vec2f tc_edge_r = v2.m_tc;
    const Vec2f tc_bottom = v3.m_tc;

    // calculate tex coord edge unit steps
    const Vec2f tc_edge_step_l = (tc_bottom - tc_edge_l) / (v3.m_pos.m_y - v1.m_pos.m_y);
    const Vec2f tc_edge_step_r = (tc_bottom - tc_edge_r) / (v3.m_pos.m_y - v2.m_pos.m_y);

    // do tex coord edge prestep
    tc_edge_l += tc_edge_step_l * (float(ystart) + .5f - v2.m_pos.m_y);
    tc_edge_r += tc_edge_step_r * (float(ystart) + .5f - v2.m_pos.m_y);

    // init tex width / heigth and clamp values
    const float tex_width = float(tex.get_width());
    const float tex_height = float(tex.get_height());
    const float tex_clamp_x = tex_width - 1.f;
    const float tex_clamp_y = tex_height - 1.f;

    for (int y = ystart; y < yend; y++,
        tc_edge_l += tc_edge_step_l, tc_edge_r += tc_edge_step_r)
    {
        // calculate start and end points (x-coords)
        // add 0.5 to y value because we're calculating based on pixel CENTERS
        const float px1 = m1 * (float(y) + .5f - v1.m_pos.m_y) + v1.m_pos.m_x;
        const float px2 = m2 * (float(y) + .5f - v2.m_pos.m_y) + v2.m_pos.m_x;

        // calculate start and end pixels
        const int xstart = (int)ceil(px1 - .5f);
        const int xend = (int)ceil(px2 - .5f); // the pixel AFTER the last pixel drawn

        // calculate tex coord scanline unit step
        const Vec2f tc_scan_step = (tc_edge_r - tc_edge_l) / (px2 - px1);

        // do tex coord scanline prestep
        Vec2f tc = tc_edge_l + tc_scan_step * (float(xstart) + .5f - px1);

        for (int x = xstart; x < xend; x++, tc += tc_scan_step) {
            put_pixel(x, y, tex.get_pixel(
                int(std::min(tc.m_x * tex_width, tex_clamp_x)),
                int(std::min(tc.m_y * tex_height, tex_clamp_y))
            ));
            // need std::min because with floating point errors
            // we could read beyond the texture edge
        }
    }
}

void Graphics::draw_textured_flat_bottom_triangle(const TexVertex &v1, const TexVertex &v2,
    const TexVertex &v3, const Surface &tex)
{
    // calculate slopes in screen space
    const float m1 = (v2.m_pos.m_x - v1.m_pos.m_x) / (v2.m_pos.m_y - v1.m_pos.m_y);
    const float m2 = (v3.m_pos.m_x - v1.m_pos.m_x) / (v3.m_pos.m_y - v1.m_pos.m_y);

    // calculate start and end scanlines
    const int ystart = (int)ceil(v1.m_pos.m_y - .5f);
    const int yend = (int)ceil(v3.m_pos.m_y - .5f);

    // init tex coord edges
    Vec2f tc_edge_l = v1.m_tc;
    Vec2f tc_edge_r = v1.m_tc;
    const Vec2f tc_bottom_l = v2.m_tc;
    const Vec2f tc_bottom_r = v3.m_tc;

    // calculate tex coord edge unit steps
    const Vec2f tc_edge_step_l = (tc_bottom_l - tc_edge_l) / (v2.m_pos.m_y - v1.m_pos.m_y);
    const Vec2f tc_edge_step_r = (tc_bottom_r - tc_edge_r) / (v3.m_pos.m_y - v1.m_pos.m_y);

    // do tex coord edge prestep
    tc_edge_l += tc_edge_step_l * (float(ystart) + .5f - v1.m_pos.m_y);
    tc_edge_r += tc_edge_step_r * (float(ystart) + .5f - v1.m_pos.m_y);

    // init tex width / heigth and clamp values
    const float tex_width = float(tex.get_width());
    const float tex_height = float(tex.get_height());
    const float tex_clamp_x = tex_width - 1.f;
    const float tex_clamp_y = tex_height - 1.f;

    for (int y = ystart; y < yend; y++,
        tc_edge_l += tc_edge_step_l, tc_edge_r += tc_edge_step_r)
    {
        // calculate start and end points (x-coords)
        // add 0.5 to y value because we're calculating based on pixel CENTERS
        const float px1 = m1 * (float(y) + .5f - v1.m_pos.m_y) + v1.m_pos.m_x;
        const float px2 = m2 * (float(y) + .5f - v1.m_pos.m_y) + v1.m_pos.m_x;

        // calculate start and end pixels
        const int xstart = (int)ceil(px1 - .5f);
        const int xend = (int)ceil(px2 - .5f); // the pixel AFTER the last pixel drawn

        // calculate tex coord scanline unit step
        const Vec2f tc_scan_step = (tc_edge_r - tc_edge_l) / (px2 - px1);

        // do tex coord scanline prestep
        Vec2f tc = tc_edge_l + tc_scan_step * (float(xstart) + .5f - px1);

        for (int x = xstart; x < xend; x++, tc += tc_scan_step) {
            put_pixel(x, y, tex.get_pixel(
                int(std::min(tc.m_x * tex_width, tex_clamp_x)),
                int(std::min(tc.m_y * tex_height, tex_clamp_y))
            ));
            // need std::min because with floating point errors
            // we could read beyond the texture edge
        }
    }
}

//////////////////////////////////////////////////
//           Graphics Exception
Graphics::Exception::Exception(HRESULT hr, const std::wstring &note,
    const wchar_t *file, unsigned int line)
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
    return    (!errorName.empty() ? std::wstring(L"Error: ") + errorName + L"\n" : empty)
        + (!errorDesc.empty() ? std::wstring(L"Description: ") + errorDesc + L"\n" : empty)
        + (!note.empty() ? std::wstring(L"Note: ") + note + L"\n" : empty)
        + (!location.empty() ? std::wstring(L"Location: ") + location : empty);
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
