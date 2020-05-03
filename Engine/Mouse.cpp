/******************************************************************************************
 *	Chili DirectX Framework Version 16.07.20											  *
 *	Mouse.cpp																			  *
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
#include "Mouse.h"

std::pair<int, int> Mouse::get_pos() const
{
    return { m_x,m_y };
}

int Mouse::get_pos_x() const
{
    return m_x;
}

int Mouse::get_pos_y() const
{
    return m_y;
}

bool Mouse::is_left_pressed() const
{
    return m_left_pressed;
}

bool Mouse::is_right_pressed() const
{
    return m_right_pressed;
}

bool Mouse::is_in_window() const
{
    return m_in_window;
}

Mouse::Event Mouse::read()
{
    if (m_buffer.size() > 0u) {
        Mouse::Event e = m_buffer.front();
        m_buffer.pop();
        return e;
    } else {
        return Mouse::Event();
    }
}

bool Mouse::is_empty() const
{
    return m_buffer.empty();
}

void Mouse::flush()
{
    m_buffer = std::queue<Event>();
}

void Mouse::on_mouse_leave()
{
    m_in_window = false;
}

void Mouse::on_mouse_enter()
{
    m_in_window = true;
}

void Mouse::on_mouse_move(int newx, int newy)
{
    m_x = newx;
    m_y = newy;

    m_buffer.push(Mouse::Event(Mouse::Event::Type::Move, *this));
    trim_buffer();
}

void Mouse::on_left_pressed(int x, int y)
{
    m_left_pressed = true;

    m_buffer.push(Mouse::Event(Mouse::Event::Type::LPress, *this));
    trim_buffer();
}

void Mouse::on_left_released(int x, int y)
{
    m_left_pressed = false;

    m_buffer.push(Mouse::Event(Mouse::Event::Type::LRelease, *this));
    trim_buffer();
}

void Mouse::on_right_pressed(int x, int y)
{
    m_right_pressed = true;

    m_buffer.push(Mouse::Event(Mouse::Event::Type::RPress, *this));
    trim_buffer();
}

void Mouse::on_right_released(int x, int y)
{
    m_right_pressed = false;

    m_buffer.push(Mouse::Event(Mouse::Event::Type::RRelease, *this));
    trim_buffer();
}

void Mouse::on_wheel_up(int x, int y)
{
    m_buffer.push(Mouse::Event(Mouse::Event::Type::WheelUp, *this));
    trim_buffer();
}

void Mouse::on_wheel_down(int x, int y)
{
    m_buffer.push(Mouse::Event(Mouse::Event::Type::WheelDown, *this));
    trim_buffer();
}

void Mouse::trim_buffer()
{
    while (m_buffer.size() > k_buffer_size)
        m_buffer.pop();
}

bool Mouse::Event::is_valid() const
{
    return m_type != Type::Invalid;
}

Mouse::Event::Type Mouse::Event::get_type() const
{
    return m_type;
}

std::pair<int, int> Mouse::Event::get_pos() const
{
    return{ m_x,m_y };
}

int Mouse::Event::get_pos_x() const
{
    return m_x;
}

int Mouse::Event::get_pos_y() const
{
    return m_y;
}

bool Mouse::Event::is_left_pressed() const
{
    return m_left_pressed;
}

bool Mouse::Event::is_right_pressed() const
{
    return m_right_pressed;
}
