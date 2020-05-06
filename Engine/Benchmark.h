#pragma once
#include <limits>
#include <string>
#include "FrameTimer.h"

class Benchmark
{
public:
    void start();
    bool end();
    operator std::wstring() const;
private:
    FrameTimer m_ft;
    int m_count = 0;
    int m_num_samples = 50;
    float m_min = std::numeric_limits<float>::max();
    float m_max = 0.f;
    float m_mean = 0.f;
    float m_prev_min = std::numeric_limits<float>::signaling_NaN();
    float m_prev_max = std::numeric_limits<float>::signaling_NaN();
    float m_prev_mean = std::numeric_limits<float>::signaling_NaN();
};
