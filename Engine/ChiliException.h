/******************************************************************************************
*	Chili DirectX Framework Version 16.07.20											  *
*	ChiliException.h																	  *
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
#include <string>

class ChiliException
{
public:
    ChiliException(const wchar_t *file, unsigned int line, const std::wstring &note = L"") :
        m_note(note), m_file(file), m_line(line)
    { }
    const std::wstring &get_note() const;
    const std::wstring &get_file() const;
    unsigned int get_line() const;
    std::wstring get_location() const;
    virtual std::wstring get_full_message() const = 0;
    virtual std::wstring get_exception_type() const = 0;
private:
    std::wstring m_note;
    std::wstring m_file;
    unsigned int m_line;
};
