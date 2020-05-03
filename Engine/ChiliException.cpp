#include "ChiliException.h"

const std::wstring &ChiliException::get_note() const
{
    return m_note;
}

const std::wstring &ChiliException::get_file() const
{
    return m_file;
}

unsigned int ChiliException::get_line() const
{
    return m_line;
}

std::wstring ChiliException::get_location() const
{
    return std::wstring(L"Line [") + std::to_wstring(m_line) + L"] in " + m_file;
}
