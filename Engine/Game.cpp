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
#include "Mat2.h"

Game::Game(MainWindow &wnd) : m_wnd(wnd), m_gfx(wnd)
{
    const float dtheta = 2.f * PI / float(k_num_flares * 2);
    for (int i = 0; i < k_num_flares * 2; i++) {
        const float rad = (i % 2 == 0) ? k_rad_outer : k_rad_inner;
        m_star.emplace_back(rad * cos(float(i) * dtheta), rad * sin(float(i) * dtheta));
    }
}

void Game::go()
{
    m_gfx.begin_frame();
    update_model();
    compose_frame();
    m_gfx.end_frame();
}

void Game::update_model()
{
    if (!m_wnd.m_kbd.is_key_pressed(VK_SPACE))
        m_theta += k_vrot;
}

void Game::compose_frame()
{
    const Vec2f trl(float(Graphics::k_screen_width) / 2.f, float(Graphics::k_screen_height) / 2.f);
    const Mat2f trf = Mat2f::rotation(m_theta) * Mat2f::scaling(k_size);
    auto vtx(m_star);
    for (auto &v : vtx) {
        v *= trf;
        v += trl;
    }
    for (auto i = vtx.cbegin(), end = std::prev(vtx.cend()); i != end; i++)
        m_gfx.draw_line(*i, *std::next(i), Colors::White);
    m_gfx.draw_line(vtx.front(), vtx.back(), Colors::White);
}
