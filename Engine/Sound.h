/******************************************************************************************
 *	Chili DirectX Framework Sound Pack Version 16.11.11									  *
 *	Sound.h																				  *
 *	Copyright 2016 PlanetChili.net <http://www.planetchili.net>							  *
 *																						  *
 *	This file is part of The Chili DirectX Framework.									  *
 *																						  *
 *	The Chili DirectX Framework is free software: you can redistribute it and/or modify	  *
 *	it under the terms of the GNU General Public License as published by				  *
 *	the Free Software Foundation, either version 3 of the License, or					  *
 *	(at your option) any later version.													  *
 *																						  *
 *	The Chili DirectX Framework is distributed in the hope that it will be useful,		  *
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of						  *
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the						  *
 *	GNU General Public License for more details.										  *
 *																						  *
 *	You should have received a copy of the GNU General Public License					  *
 *	along with this source code.  If not, see <http://www.gnu.org/licenses/>.			  *
 ******************************************************************************************/
#pragma once
#include "ChiliWin.h"
#include <memory>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <thread>
#include "ChiliException.h"
#include <wrl\client.h>
#include "COMInitializer.h"

 // forward declare WAVEFORMATEX so we don't have to include bullshit headers
struct tWAVEFORMATEX;
typedef tWAVEFORMATEX WAVEFORMATEX;

class SoundSystem
{
public:
    class APIException : public ChiliException
    {
    public:
        APIException(HRESULT hr, const wchar_t *file, unsigned int line, const std::wstring &note);
        std::wstring get_error_name() const;
        std::wstring get_error_description() const;
        virtual std::wstring get_full_message() const override;
        virtual std::wstring get_exception_type() const override;
    private:
        HRESULT m_hr;
    };
    class FileException : public ChiliException
    {
    public:
        FileException(const wchar_t *file, unsigned int line, const std::wstring &note, const std::wstring &filename);
        virtual std::wstring get_full_message() const override;
        virtual std::wstring get_exception_type() const override;
    private:
        std::wstring m_filename;
    };
private:
    class MFInitializer
    {
    public:
        MFInitializer();
        ~MFInitializer();
    private:
        HRESULT m_hr;
    };
    class XAudioDll
    {
    private:
        enum class LoadType
        {
            Folder,
            Local,
            System,
            Invalid
        };
    public:
        XAudioDll();
        ~XAudioDll();
        operator HMODULE() const;
    private:
        static const wchar_t *get_dll_path(LoadType type);
    private:
        HMODULE m_hmodule = 0;
        static constexpr wchar_t *k_system_path = L"XAudio2_7.dll";
#ifdef _M_X64
        static constexpr wchar_t *k_folder_path = L"XAudio\\XAudio2_7_64.dll";
        static constexpr wchar_t *k_local_path = L"XAudio2_7_64.dll";
#else
        static constexpr wchar_t *k_folder_path = L"XAudio\\XAudio2_7_32.dll";
        static constexpr wchar_t *k_local_path = L"XAudio2_7_32.dll";
#endif
    };
public:
    class Channel
    {
        friend class Sound;
    public:
        Channel(SoundSystem &sys);
        Channel(const Channel &) = delete;
        ~Channel();
        void play_sound_buffer(class Sound &s, float freqMod, float vol);
        void stop();
    private:
        void retarget_sound(const Sound *pOld, Sound *pNew);
    private:
        std::unique_ptr<struct XAUDIO2_BUFFER> m_xa_buffer;
        struct IXAudio2SourceVoice *m_src = nullptr;
        class Sound *m_sound = nullptr;
    };
public:
    SoundSystem(const SoundSystem &) = delete;
    static SoundSystem &get();
    static void set_master_volume(float vol = 1.0f);
    static const WAVEFORMATEX &get_format();
    void play_sound_buffer(class Sound &s, float freqMod, float vol);
private:
    SoundSystem();
    void deactivate_channel(Channel &channel);
private:
    COMInitializer m_com_init;
    MFInitializer m_mf_init;
    XAudioDll m_xaudio_dll;
    Microsoft::WRL::ComPtr<struct IXAudio2> m_engine_ptr;
    struct IXAudio2MasteringVoice *m_master = nullptr;
    std::unique_ptr<WAVEFORMATEX> m_format;
    std::mutex m_mutex;
    std::vector<std::unique_ptr<Channel>> m_idle_channel_ptrs;
    std::vector<std::unique_ptr<Channel>> m_active_channel_ptrs;
private:
    // change these values to match the format of the wav files you are loading
    // all wav files must have the same format!! (no mixing and matching)
    static constexpr WORD k_num_channnels_per_sound = 2u;
    static constexpr DWORD k_num_samples_per_sec = 44100u;
    static constexpr WORD k_num_bits_per_sample = 16u;
    // change this value to increase/decrease the maximum polyphony
    static constexpr size_t k_num_channels = 64u;
};

class Sound
{
    friend SoundSystem::Channel;
public:
    enum class LoopType
    {
        NotLooping,
        AutoEmbeddedCuePoints,
        AutoFullSound,
        ManualFloat,
        ManualSample,
        Invalid
    };
public:
    Sound() = default;
    // for backwards compatibility--2nd parameter false -> NotLooping (does not work with non-wav)
    Sound(const std::wstring &fileName, bool loopingWithAutoCueDetect);
    // do not pass this function Manual LoopTypes!
    Sound(const std::wstring &fileName, LoopType loopType = LoopType::NotLooping);
    Sound(const std::wstring &fileName, unsigned int loopStart, unsigned int loopEnd);
    Sound(const std::wstring &fileName, float loopStart, float loopEnd);
    Sound(Sound &&donor) noexcept;
    Sound &operator=(Sound &&donor) noexcept;
    void play(float freqMod = 1.0f, float vol = 1.0f);
    void stop_one();
    void stop_all();
    ~Sound();
private:
    static Sound load_non_wav(const std::wstring &fileName, LoopType loopType,
        unsigned int loopStartSample, unsigned int loopEndSample,
        float loopStartSeconds, float loopEndSeconds);
    Sound(const std::wstring &fileName, LoopType loopType,
        unsigned int loopStartSample, unsigned int loopEndSample,
        float loopStartSeconds, float loopEndSeconds);
private:
    UINT32 m_num_bytes = 0u;
    bool m_looping = false;
    unsigned int m_loop_start = 0;
    unsigned int m_loop_end = 0;
    std::unique_ptr<BYTE[]> m_data;
    std::mutex m_mutex;
    std::condition_variable m_cv_death;
    std::vector<SoundSystem::Channel *> m_active_channel_ptrs;
    static constexpr unsigned int k_null_sample = 0xFFFFFFFFu;
    static constexpr float k_null_seconds = -1.0f;
};
