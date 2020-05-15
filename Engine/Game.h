/******************************************************************************************
 *	Chili DirectX Framework Version 16.07.20											  *
 *	Game.h																				  *
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
#pragma once
#include "Graphics.h"
#include "Vec2.h"
#include <vector>
#include "ChiliMath.h"

class Game
{
public:
    Game(class MainWindow &wnd);
    Game(const Game &) = delete;
    Game &operator=(const Game &) = delete;
    void go();
private:
    void compose_frame();
    void update_model();
    /********************************/
    /*  User Functions              */
    /********************************/
private:
    MainWindow &m_wnd;
    Graphics m_gfx;
    /********************************/
    /*  User Variables              */
    std::vector<Vec2f> m_star;
    float m_theta = 0.f;
    static constexpr float k_vrot = PI / 60.f;
    static constexpr float k_rad_inner = 1.f;
    static constexpr float k_rad_outer = 2.f;
    static constexpr int k_num_flares = 5;
    static constexpr float k_size = 100.f;
    /********************************/
};
