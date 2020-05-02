#include "ChiliException.h"

const std::wstring &ChiliException::GetNote() const
{
    return note;
}

const std::wstring &ChiliException::GetFile() const
{
    return file;
}

unsigned int ChiliException::GetLine() const
{
    return line;
}

std::wstring ChiliException::GetLocation() const
{
    return std::wstring(L"Line [") + std::to_wstring(line) + L"] in " + file;
}
