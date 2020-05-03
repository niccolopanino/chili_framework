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

Game::Game(MainWindow &wnd) : m_wnd(wnd), m_gfx(wnd)
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
    // process key messages while any remain
    while (!m_wnd.m_kbd.is_key_empty()) {
        const auto e = m_wnd.m_kbd.read_key();
        // only interested in space bar presses
        if (e.is_press() && e.get_code() == VK_SPACE) {
            m_link.activate_effect();
            m_hit_sound.play();
        }
    }

    // process arrow keys state
    Vec2f dir = { 0.f, 0.f };
    if (m_wnd.m_kbd.is_key_pressed(VK_UP))
        dir.m_y -= 1.f;
    if (m_wnd.m_kbd.is_key_pressed(VK_DOWN))
        dir.m_y += 1.f;
    if (m_wnd.m_kbd.is_key_pressed(VK_LEFT))
        dir.m_x -= 1.f;
    if (m_wnd.m_kbd.is_key_pressed(VK_RIGHT))
        dir.m_x += 1.f;

    m_link.set_direction(dir);
    m_link.update(m_ft.mark());
}

void Game::compose_frame()
{
    m_link.draw(m_gfx);
    m_font.draw_text("Becky.\nLemme smash.", m_wnd.m_mouse.get_pos(), Colors::White, m_gfx);
}
