/******************************************************************************************
 *	Chili DirectX Framework Version 16.07.20											  *
 *	Mouse.h																				  *
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
#include <queue>
#include "Vec2.h"

class Mouse
{
    friend class MainWindow;
public:
    class Event
    {
    public:
        enum class Type
        {
            LPress,
            LRelease,
            RPress,
            RRelease,
            WheelUp,
            WheelDown,
            Move,
            Invalid
        };
    private:
        Type m_type;
        bool m_left_pressed;
        bool m_right_pressed;
        int m_x;
        int m_y;
    public:
        Event() :
            m_type(Type::Invalid),
            m_left_pressed(false), m_right_pressed(false),
            m_x(0), m_y(0)
        { }
        Event(Type type, const Mouse &parent) :
            m_type(type),
            m_left_pressed(parent.m_left_pressed), m_right_pressed(parent.m_right_pressed),
            m_x(parent.m_x), m_y(parent.m_y)
        { }
        bool is_valid() const;
        Type get_type() const;
        Vec2i get_pos() const;
        int get_pos_x() const;
        int get_pos_y() const;
        bool is_left_pressed() const;
        bool is_right_pressed() const;
    };
public:
    Mouse() = default;
    Mouse(const Mouse &) = delete;
    Mouse &operator=(const Mouse &) = delete;
    Vec2i get_pos() const;
    int get_pos_x() const;
    int get_pos_y() const;
    bool is_left_pressed() const;
    bool is_right_pressed() const;
    bool is_in_window() const;
    Mouse::Event read();
    bool is_empty() const;
    void flush();
private:
    void on_mouse_move(int x, int y);
    void on_mouse_leave();
    void on_mouse_enter();
    void on_left_pressed(int x, int y);
    void on_left_released(int x, int y);
    void on_right_pressed(int x, int y);
    void on_right_released(int x, int y);
    void on_wheel_up(int x, int y);
    void on_wheel_down(int x, int y);
    void trim_buffer();
private:
    static constexpr unsigned int k_buffer_size = 4u;
    int m_x = 0;
    int m_y = 0;
    bool m_left_pressed = false;
    bool m_right_pressed = false;
    bool m_in_window = false;
    std::queue<Event> m_buffer;
};
