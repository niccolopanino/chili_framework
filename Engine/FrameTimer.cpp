#include "FrameTimer.h"

FrameTimer::FrameTimer()
{
    m_last = std::chrono::steady_clock::now();
}

float FrameTimer::mark()
{
    const auto old = m_last;
    m_last = std::chrono::steady_clock::now();
    const std::chrono::duration<float> frame_time = m_last - old;
    return frame_time.count();
}
