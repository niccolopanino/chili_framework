#define FULL_WINTARD
#include "ChiliWin.h"
#include "GDIPlusManager.h"
#include <algorithm>
namespace Gdiplus
{
    using std::min;
    using std::max;
}
#include <gdiplus.h>

#pragma comment(lib, "gdiplus.lib")

ULONG_PTR GDIPlusManager::m_token = 0;
int GDIPlusManager::m_ref_count = 0;

GDIPlusManager::GDIPlusManager()
{
    if (GDIPlusManager::m_ref_count++ == 0) {
        Gdiplus::GdiplusStartupInput input;
        input.GdiplusVersion = 1;
        input.DebugEventCallback = nullptr;
        input.SuppressBackgroundThread = false;
        Gdiplus::GdiplusStartup(&GDIPlusManager::m_token, &input, nullptr);
    }
}

GDIPlusManager::~GDIPlusManager()
{
    if (--GDIPlusManager::m_ref_count == 0)
        Gdiplus::GdiplusShutdown(GDIPlusManager::m_token);
}
