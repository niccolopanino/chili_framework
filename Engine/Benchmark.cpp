#include "Benchmark.h"
#include <sstream>
#include <algorithm>

void Benchmark::start()
{
    m_ft.mark();
}

bool Benchmark::end()
{
    const float dt = m_ft.mark();
    m_min = std::min(dt, m_min);
    m_max = std::max(dt, m_max);
    m_mean += dt / m_num_samples;

    if (++m_count >= m_num_samples)
    {
        // store results
        m_prev_min = m_min;
        m_prev_max = m_max;
        m_prev_mean = m_mean;
        // reset bench state
        m_count = 0;
        m_min = std::numeric_limits<float>::max();
        m_max = 0.f;
        m_mean = 0.f;
        // signal results update
        return true;
    }
    return false;
}

Benchmark::operator std::wstring() const
{
    std::wostringstream wos;
    wos << L"Sample size: " << m_num_samples << std::endl
        << L"Mean: " << m_prev_mean * 1000.f << L"ms" << std::endl
        << L"Min: " << m_prev_min * 1000.f << L"ms" << std::endl
        << L"Max: " << m_prev_max * 1000.f << L"ms" << std::endl;
    return wos.str();
}
