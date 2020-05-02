#include "SoundEffect.h"

SoundEffect::SoundEffect(const std::initializer_list<std::wstring> &wavFiles,
    bool soft_fail, float freqStdDevFactor)
    :
    freqDist(0.0f, freqStdDevFactor),
    soundDist(0, unsigned int(wavFiles.size() - 1))
{
    sounds.reserve(wavFiles.size());
    for (auto &f : wavFiles)
    {
        try
        {
            sounds.emplace_back(f);
        } catch (const SoundSystem::FileException &e)
        {
            if (soft_fail)
            {
                sounds.emplace_back();
            } else
            {
                throw e;
            }
        }
    }
}
