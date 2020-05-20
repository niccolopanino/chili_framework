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
#include "Cube.h"
#include "Sphere.h"
#include "CubeSkinScene.h"
#include "CubeVertexColorScene.h"
#include "CubeSolidScene.h"
#include "DoubleCubeScene.h"
#include "VertexWaveScene.h"
#include "CubeVertexPositionColorScene.h"
#include "CubeSolidGeometryScene.h"
#include "CubeFlatIndependentScene.h"
#include "GeometryFlatScene.h"
#include "GouraudScene.h"
#include <sstream>

Game::Game(MainWindow &wnd) : m_wnd(wnd), m_gfx(wnd)
{
    m_scenes.push_back(std::make_unique<CubeSolidScene>(m_gfx));
    m_scenes.push_back(std::make_unique<CubeSolidGeometryScene>(m_gfx));
    m_scenes.push_back(std::make_unique<CubeVertexColorScene>(m_gfx));
    m_scenes.push_back(std::make_unique<CubeVertexPositionColorScene>(m_gfx));
    m_scenes.push_back(std::make_unique<CubeSkinScene>(m_gfx, L"resources/img/office_skin.jpg"));
    m_scenes.push_back(std::make_unique<DoubleCubeScene>(m_gfx));
    m_scenes.push_back(std::make_unique<VertexWaveScene>(m_gfx));
    m_scenes.push_back(std::make_unique<CubeFlatIndependentScene>(m_gfx));
    m_scenes.push_back(std::make_unique<GeometryFlatScene>(m_gfx,
        Cube::get_plain<GeometryFlatScene::Vertex>()));
    m_scenes.push_back(std::make_unique<GeometryFlatScene>(m_gfx,
        Sphere::get_plain<GeometryFlatScene::Vertex>()));
    m_scenes.push_back(std::make_unique<GouraudScene>(m_gfx,
        Sphere::get_plain_normals<GouraudScene::Vertex>()));
    m_scenes.push_back(std::make_unique<GeometryFlatScene>(m_gfx,
        IndexedTriangleList<GeometryFlatScene::Vertex>::load("resources/models/bunny.obj")));
    m_scenes.push_back(std::make_unique<GouraudScene>(m_gfx,
        IndexedTriangleList<GouraudScene::Vertex>::load_normals("resources/models/suzanne.obj")));
    m_cur_scene = m_scenes.begin();
    output_scene_name();
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
    const float dt = m_ft.mark();
    // cycle through scenes when tab is pressed
    while (!m_wnd.m_kbd.is_key_empty()) {
        const auto e = m_wnd.m_kbd.read_key();
        if (e.get_code() == VK_TAB && e.is_press())
        {
            if (m_wnd.m_kbd.is_key_pressed(VK_SHIFT))
                reverse_cycle_scenes();
            else
                cycle_scenes();
        }
        else if (e.get_code() == VK_ESCAPE && e.is_press())
            m_wnd.kill();
    }
    // update scene
    (*m_cur_scene)->update(m_wnd.m_kbd, m_wnd.m_mouse, dt);
}

void Game::cycle_scenes()
{
    if (++m_cur_scene == m_scenes.end())
        m_cur_scene = m_scenes.begin();
    output_scene_name();
}

void Game::reverse_cycle_scenes()
{
    if (m_cur_scene == m_scenes.begin())
        m_cur_scene = m_scenes.end() - 1;
    else
        --m_cur_scene;
    output_scene_name();
}

void Game::output_scene_name() const
{
    std::stringstream ssm;
    const std::string stars((*m_cur_scene)->get_name().size() + 4, '*');
    ssm << stars << std::endl
        << "* " << (*m_cur_scene)->get_name() << " *" << std::endl
        << stars << std::endl;
    OutputDebugStringA(ssm.str().c_str());
}

void Game::compose_frame()
{
    // draw current scene
    (*m_cur_scene)->draw();
}
