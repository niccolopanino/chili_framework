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
#include "PubeScreenTransformer.h"

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
{ }

void Game::compose_frame()
{
    PubeScreenTransformer pms;
    Vec3f v0(0.f, .5f, 0.f);
    Vec3f v1(.5f, -.5f, 0.f);
    Vec3f v2(-.5f, -.5f, 0.f);
    m_gfx.draw_line(pms.get_transformed(v0), pms.get_transformed(v1), Colors::White);
    m_gfx.draw_line(pms.get_transformed(v1), pms.get_transformed(v2), Colors::White);
    m_gfx.draw_line(pms.get_transformed(v2), pms.get_transformed(v0), Colors::White);
}
