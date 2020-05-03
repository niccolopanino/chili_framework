#include "COMInitializer.h"
#include <objbase.h>

COMInitializer::COMInitializer()
{
    m_hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
}

COMInitializer::~COMInitializer()
{
    if (m_hr == S_OK)
        CoUninitialize();
}
