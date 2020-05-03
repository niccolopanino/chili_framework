/******************************************************************************************
 *	Chili DirectX Framework Version 16.07.20											  *
 *	Keyboard.cpp																		  *
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
#include "Keyboard.h"

bool Keyboard::is_key_pressed(unsigned char keycode) const
{
    return m_keystates[keycode];
}

Keyboard::Event Keyboard::read_key()
{
    if (m_keybuffer.size() > 0u) {
        Keyboard::Event e = m_keybuffer.front();
        m_keybuffer.pop();
        return e;
    } else {
        return Keyboard::Event();
    }
}

bool Keyboard::is_key_empty() const
{
    return m_keybuffer.empty();
}

char Keyboard::read_char()
{
    if (m_charbuffer.size() > 0u) {
        unsigned char charcode = m_charbuffer.front();
        m_charbuffer.pop();
        return charcode;
    } else {
        return 0;
    }
}

bool Keyboard::is_char_empty() const
{
    return m_charbuffer.empty();
}

void Keyboard::flush_key()
{
    m_keybuffer = std::queue<Event>();
}

void Keyboard::flush_char()
{
    m_charbuffer = std::queue<char>();
}

void Keyboard::flush()
{
    flush_key();
    flush_char();
}

void Keyboard::enable_autorepeat()
{
    m_autorepeat_enabled = true;
}

void Keyboard::disable_autorepeat()
{
    m_autorepeat_enabled = false;
}

bool Keyboard::is_autorepeat_enabled() const
{
    return m_autorepeat_enabled;
}

void Keyboard::on_key_pressed(unsigned char keycode)
{
    m_keystates[keycode] = true;
    m_keybuffer.push(Keyboard::Event(Keyboard::Event::Type::Press, keycode));
    trim_buffer(m_keybuffer);
}

void Keyboard::on_key_released(unsigned char keycode)
{
    m_keystates[keycode] = false;
    m_keybuffer.push(Keyboard::Event(Keyboard::Event::Type::Release, keycode));
    trim_buffer(m_keybuffer);
}

void Keyboard::on_char(char character)
{
    m_charbuffer.push(character);
    trim_buffer(m_charbuffer);
}

void Keyboard::clear_state()
{
    m_keystates.reset();
}

template<typename T>
void Keyboard::trim_buffer(std::queue<T> &buffer)
{
    while (buffer.size() > k_buffer_size)
        buffer.pop();
}

bool Keyboard::Event::is_press() const
{
    return m_type == Type::Press;
}

bool Keyboard::Event::is_release() const
{
    return m_type == Type::Release;
}

bool Keyboard::Event::is_valid() const
{
    return m_type != Type::Invalid;
}

unsigned char Keyboard::Event::get_code() const
{
    return m_code;
}
