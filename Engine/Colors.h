/******************************************************************************************
*	Chili DirectX Framework Version 16.07.20											  *
*	Colors.h																			  *
*	Copyright 2016 PlanetChili <http://www.planetchili.net>								  *
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
#include "Color.h"

namespace Colors
{
    static constexpr Color make_rgb(unsigned char r, unsigned char g, unsigned char b)
    {
        return (r << 16) | (g << 8) | b;
    }
    static constexpr Color White = make_rgb(255u, 255u, 255u);
    static constexpr Color Black = make_rgb(0u, 0u, 0u);
    static constexpr Color Gray = make_rgb(0x80u, 0x80u, 0x80u);
    static constexpr Color LightGray = make_rgb(0xD3u, 0xD3u, 0xD3u);
    static constexpr Color Red = make_rgb(255u, 0u, 0u);
    static constexpr Color Green = make_rgb(0u, 255u, 0u);
    static constexpr Color Blue = make_rgb(0u, 0u, 255u);
    static constexpr Color Yellow = make_rgb(255u, 255u, 0u);
    static constexpr Color Cyan = make_rgb(0u, 255u, 255u);
    static constexpr Color Magenta = make_rgb(255u, 0u, 255u);
}
