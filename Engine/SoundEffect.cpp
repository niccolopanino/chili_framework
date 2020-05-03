#include "SoundEffect.h"

SoundEffect::SoundEffect(const std::initializer_list<std::wstring> &wavFiles,
    bool soft_fail, float freqStdDevFactor)
    :
    m_freq_dist(0.0f, freqStdDevFactor),
    m_sound_dist(0, unsigned int(wavFiles.size() - 1))
{
    m_sounds.reserve(wavFiles.size());
    for (auto &f : wavFiles)
    {
        try
        {
            m_sounds.emplace_back(f);
        } catch (const SoundSystem::FileException &e)
        {
            if (soft_fail)
            {
                m_sounds.emplace_back();
            } else
            {
                throw e;
            }
        }
    }
}

template<class T>
inline void SoundEffect::play(T &rng, float vol)
{
    m_sounds[m_sound_dist(rng)].play(exp2(m_freq_dist(rng)), vol);
}
