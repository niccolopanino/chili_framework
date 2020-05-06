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
#include <random>
#include "SpriteEffect.h"

Game::Game(MainWindow &wnd) : m_wnd(wnd), m_gfx(wnd)
{
    std::mt19937 rng(69);
    std::uniform_int_distribution<int> xd(0, Graphics::k_screen_width - m_surf.get_width() - 1);
    std::uniform_int_distribution<int> yd(0, Graphics::k_screen_height - m_surf.get_height() - 1);
    for (int i = 0; i < 50; i++)
        m_positions.push_back({ xd(rng), yd(rng) });
}

void Game::go()
{
    m_gfx.begin_frame();
    update_model();
    compose_frame();
    m_gfx.end_frame();
}

void Game::update_model()
{ }

void Game::compose_frame()
{
    m_bencher.start();

    for (const auto &pos : m_positions)
        m_gfx.draw_sprite(pos.m_x, pos.m_y, m_surf, SpriteEffect::Copy());

    if (m_bencher.end())
        OutputDebugStringW((std::wstring(m_bencher) + L"\n").c_str());
}
