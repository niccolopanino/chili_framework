#pragma once
#include "ChiliWin.h"

class GDIPlusManager
{
public:
    GDIPlusManager();
    ~GDIPlusManager();
private:
    static ULONG_PTR m_token;
    static int m_ref_count;
};
