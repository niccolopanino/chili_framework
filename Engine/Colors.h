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
    static constexpr Color MakeRGB(unsigned char r, unsigned char g, unsigned char b)
    {
        return (r << 16) | (g << 8) | b;
    }
    static constexpr Color White = MakeRGB(255u, 255u, 255u);
    static constexpr Color Black = MakeRGB(0u, 0u, 0u);
    static constexpr Color Gray = MakeRGB(0x80u, 0x80u, 0x80u);
    static constexpr Color LightGray = MakeRGB(0xD3u, 0xD3u, 0xD3u);
    static constexpr Color Red = MakeRGB(255u, 0u, 0u);
    static constexpr Color Green = MakeRGB(0u, 255u, 0u);
    static constexpr Color Blue = MakeRGB(0u, 0u, 255u);
    static constexpr Color Yellow = MakeRGB(255u, 255u, 0u);
    static constexpr Color Cyan = MakeRGB(0u, 255u, 255u);
    static constexpr Color Magenta = MakeRGB(255u, 0u, 255u);
}
