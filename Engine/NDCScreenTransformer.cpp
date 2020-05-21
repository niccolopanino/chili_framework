#include "NDCScreenTransformer.h"

NDCScreenTransformer::NDCScreenTransformer() :
    m_xfac(float(Graphics::k_screen_width) / 2.f),
    m_yfac(float(Graphics::k_screen_height) / 2.f)
{ }
