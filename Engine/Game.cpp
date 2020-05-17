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
#include "SolidCubeScene.h"
#include "CubeOrderScene.h"
#include "ConcaHexScene.h"
#include "ConcaHexWireScene.h"
#include "XMutualScene.h"
#include "TexCubeScene.h"
#include "TexWrapCubeScene.h"

Game::Game(MainWindow &wnd) : m_wnd(wnd), m_gfx(wnd)
{
    m_scenes.push_back(std::make_unique<SolidCubeScene>());
    m_scenes.push_back(std::make_unique<CubeOrderScene>());
    m_scenes.push_back(std::make_unique<ConcaHexScene>());
    m_scenes.push_back(std::make_unique<ConcaHexWireScene>());
    m_scenes.push_back(std::make_unique<XMutualScene>());
    m_scenes.push_back(std::make_unique<TexCubeScene>());
    m_scenes.push_back(std::make_unique<TexWrapCubeScene>());
    m_cur_scene = m_scenes.begin();
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
    const float dt = 1.f / 60.f;
    // cycle through scenes when tab is pressed
    while (!m_wnd.m_kbd.is_key_empty()) {
        const auto e = m_wnd.m_kbd.read_key();
        if (e.get_code() == VK_TAB && e.is_press())
            cycle_scenes();
    }
    // update scene
    (*m_cur_scene)->update(m_wnd.m_kbd, m_wnd.m_mouse, dt);
}

void Game::cycle_scenes()
{
    if (++m_cur_scene == m_scenes.end())
        m_cur_scene = m_scenes.begin();
}

void Game::compose_frame()
{
    // draw current scene
    (*m_cur_scene)->draw(m_gfx);
}
