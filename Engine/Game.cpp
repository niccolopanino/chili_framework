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
    auto lines = m_cube.get_lines();
    Mat3f rot = Mat3f::rotate_x(m_theta_x)
        * Mat3f::rotate_y(m_theta_y)
        * Mat3f::rotate_z(m_theta_z);
    for (auto &v : lines.m_vertices) {
        v *= rot;
        v += Vec3f(0.f, 0.f, m_offset_z);
        m_pms.transform(v);
    }
    for (auto i = lines.m_indices.cbegin(), end = lines.m_indices.cend();
        i != end;
        std::advance(i, 2))
    {
        m_gfx.draw_line(lines.m_vertices[*i], lines.m_vertices[*std::next(i)], Colors::White);
    }
}
