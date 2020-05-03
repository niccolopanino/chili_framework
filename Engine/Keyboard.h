/******************************************************************************************
 *	Chili DirectX Framework Version 16.07.20											  *
 *	Keyboard.h																			  *
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
#include <queue>
#include <bitset>

class Keyboard
{
    friend class MainWindow;
public:
    class Event
    {
    public:
        enum class Type
        {
            Press,
            Release,
            Invalid
        };
    private:
        Type m_type;
        unsigned char m_code;
    public:
        Event() : m_type(Type::Invalid), m_code(0u) { }
        Event(Type type, unsigned char code) : m_type(type), m_code(code) { }
        bool is_press() const;
        bool is_release() const;
        bool is_valid() const;
        unsigned char get_code() const;
    };
public:
    Keyboard() = default;
    Keyboard(const Keyboard &) = delete;
    Keyboard &operator=(const Keyboard &) = delete;
    bool is_key_pressed(unsigned char keycode) const;
    Event read_key();
    bool is_key_empty() const;
    char read_char();
    bool is_char_empty() const;
    void flush_key();
    void flush_char();
    void flush();
    void enable_autorepeat();
    void disable_autorepeat();
    bool is_autorepeat_enabled() const;
private:
    void on_key_pressed(unsigned char keycode);
    void on_key_released(unsigned char keycode);
    void on_char(char character);
    void clear_state();
    template<typename T>
    void trim_buffer(std::queue<T> &buffer);
private:
    static constexpr unsigned int k_nkeys = 256u;
    static constexpr unsigned int k_buffer_size = 4u;
    bool m_autorepeat_enabled = false;
    std::bitset<k_nkeys> m_keystates;
    std::queue<Event> m_keybuffer;
    std::queue<char> m_charbuffer;
};
