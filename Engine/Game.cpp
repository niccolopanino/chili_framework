/******************************************************************************************
 *	Chili DirectX Framework Version 16.07.20											  *
 *	Game.cpp																			  *
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
#include "Game.h"
#include "Mat3.h"

Game::Game(MainWindow &wnd) : m_wnd(wnd), m_gfx(wnd), m_cube(1.f)
{ }

void Game::go()
{
    m_gfx.begin_frame();
    update_model();
    compose_frame();
    m_gfx.end_frame();
}

void Game::update_model()
{
    const float dt = 1.f / 60.f;
    if (m_wnd.m_kbd.is_key_pressed('Q'))
        m_theta_x = wrap_angle(m_theta_x + k_dtheta * dt);
    if (m_wnd.m_kbd.is_key_pressed('W'))
        m_theta_y = wrap_angle(m_theta_y + k_dtheta * dt);
    if (m_wnd.m_kbd.is_key_pressed('E'))
        m_theta_z = wrap_angle(m_theta_z + k_dtheta * dt);
    if (m_wnd.m_kbd.is_key_pressed('A'))
        m_theta_x = wrap_angle(m_theta_x - k_dtheta * dt);
    if (m_wnd.m_kbd.is_key_pressed('S'))
        m_theta_y = wrap_angle(m_theta_y - k_dtheta * dt);
    if (m_wnd.m_kbd.is_key_pressed('D'))
        m_theta_z = wrap_angle(m_theta_z - k_dtheta * dt);
    if (m_wnd.m_kbd.is_key_pressed('R'))
        m_offset_z += 2.f * dt;
    if (m_wnd.m_kbd.is_key_pressed('F'))
        m_offset_z -= 2.f * dt;
}

void Game::compose_frame()
{
    const Color colors[12] = {
        Colors::White, Colors::Blue, Colors::Cyan, Colors::Gray,
        Colors::Green, Colors::Magenta, Colors::LightGray, Colors::Red,
        Colors::Yellow, Colors::White, Colors::Blue, Colors::Cyan
    };
    // generate indexed triangle list
    auto triangles = m_cube.get_triangles();
    // generate rotation matrix from euler angles
    Mat3f rot = Mat3f::rotate_x(m_theta_x)
        * Mat3f::rotate_y(m_theta_y)
        * Mat3f::rotate_z(m_theta_z);
    // transform from model space to world / view space
    for (auto &v : triangles.m_vertices) {
        v *= rot;
        v += Vec3f(0.f, 0.f, m_offset_z);
    }
    // backface culling test (must be done in world / view space)
    for (size_t i = 0, end = triangles.m_indices.size() / 3; i < end; i++) {
        const Vec3f &v1 = triangles.m_vertices[triangles.m_indices[i * 3]];
        const Vec3f &v2 = triangles.m_vertices[triangles.m_indices[i * 3 + 1]];
        const Vec3f &v3 = triangles.m_vertices[triangles.m_indices[i * 3 + 2]];
        triangles.m_cull_flags[i] = Vec3f::dot(Vec3f::cross(v2 - v1, v3 - v1), v1) > 0.f;
    }
    // transform to screen space (includes perspective transform)
    for (auto &v : triangles.m_vertices)
        m_pms.transform(v);
    // draw the mf triangles!
    for (size_t i = 0, end = triangles.m_indices.size() / 3; i < end; i++)
    {
        // skip triangles previously determined to be back-facing
        if (!triangles.m_cull_flags[i]) {
            m_gfx.draw_triangle(triangles.m_vertices[triangles.m_indices[i * 3]],
                triangles.m_vertices[triangles.m_indices[i * 3 + 1]],
                triangles.m_vertices[triangles.m_indices[i * 3 + 2]],
                colors[i]);
        }
    }
}
