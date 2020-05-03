/******************************************************************************************
 *	Chili DirectX Framework Sound Pack Version 16.11.11									  *
 *	Sound.cpp																			  *
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
#include "Sound.h"
#include <assert.h>
#include <algorithm>
#include <fstream>
#include <array>
#include <functional>
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <mferror.h>
#include <Propvarutil.h>
#include "XAudio\XAudio2.h"
#include "DXErr.h"

#pragma comment( lib,"mfplat.lib" )
#pragma comment( lib,"mfreadwrite.lib" )
#pragma comment( lib,"mfuuid.lib" )
#pragma comment( lib,"Propsys.lib" )

#define CHILI_SOUND_API_EXCEPTION( hr,note ) SoundSystem::APIException( hr,_CRT_WIDE(__FILE__),__LINE__,note )
#define CHILI_SOUND_FILE_EXCEPTION( filename,note ) SoundSystem::FileException( _CRT_WIDE(__FILE__),__LINE__,note,filename )

SoundSystem &SoundSystem::get()
{
    static SoundSystem instance;
    return instance;
}

void SoundSystem::set_master_volume(float vol)
{
    HRESULT hr;
    if (FAILED(hr = get().m_master->SetVolume(vol)))
        throw CHILI_SOUND_API_EXCEPTION(hr, L"Setting master volume");
}

const WAVEFORMATEX &SoundSystem::get_format()
{
    return *get().m_format;
}

void SoundSystem::play_sound_buffer(Sound &s, float freqMod, float vol)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_idle_channel_ptrs.size() > 0) {
        m_active_channel_ptrs.push_back(std::move(m_idle_channel_ptrs.back()));
        m_idle_channel_ptrs.pop_back();
        m_active_channel_ptrs.back()->play_sound_buffer(s, freqMod, vol);
    }
}

SoundSystem::XAudioDll::XAudioDll()
{
    LoadType type = LoadType::System;
    while (true)
    {
        m_hmodule = LoadLibrary(get_dll_path(type));
        if (m_hmodule != 0) {
            return;
        } else {
            switch (type)
            {
                case LoadType::System:
                    type = LoadType::Folder;
                    break;
                case LoadType::Folder:
                    type = LoadType::Local;
                    break;
                case LoadType::Local:
                    throw CHILI_SOUND_API_EXCEPTION(
                        HRESULT_FROM_WIN32(GetLastError()),
                        std::wstring(
                            L"The XAudio2 DLL Could not be loaded. It is required that:\n"
                            L"A) [ ") + std::wstring(get_dll_path(LoadType::Folder)) +
                        std::wstring(L" ] exist in the same folder as this executable;\n"
                            L"B) [ ") + std::wstring(get_dll_path(LoadType::Local)) +
                        std::wstring(L" ] exist in the same folder as this executable; or\n"
                            L"C) [ XAudio2_7.dll ] be installed on this system via the DirectX"
                            L" Redistributable Installer Version June 2010\n"));
                default:
                    assert(false && "Bad LoadType encountered in XAudio Dll loading sequence loop");
            }
        }
    }
}

SoundSystem::XAudioDll::~XAudioDll()
{
    if (m_hmodule != 0) {
        FreeLibrary(m_hmodule);
        m_hmodule = 0;
    }
}

SoundSystem::XAudioDll::operator HMODULE() const
{
    return m_hmodule;
}

const wchar_t *SoundSystem::XAudioDll::get_dll_path(LoadType type)
{
    switch (type)
    {
        case LoadType::System:
            return k_system_path;
        case LoadType::Folder:
            return k_folder_path;
        case LoadType::Local:
            return k_local_path;
        default:
            assert(false && "Bad LoadType in get_dll_path function");
            return nullptr;
    }
}

SoundSystem::SoundSystem() : m_format(std::make_unique<WAVEFORMATEX>())
{
    // setup wave format info structure
    static_assert(k_num_channnels_per_sound > 0u, "WAVE File Format Error: At least one channel required per audio stream");
    static_assert(k_num_channnels_per_sound <= XAUDIO2_MAX_AUDIO_CHANNELS, "WAVE File Format Error: Maximum audio channels per audio stream exceeded");
    static_assert(k_num_samples_per_sec >= XAUDIO2_MIN_SAMPLE_RATE, "WAVE File Format Error: Sample rate lower than minimum allowed");
    static_assert(k_num_samples_per_sec <= XAUDIO2_MAX_SAMPLE_RATE, "WAVE File Format Error: Sample rate exceeds maximum allowed");
    static_assert(k_num_bits_per_sample > 0u, "WAVE File Format Error: Bit depth of 0 bits per sample is not allowed");
    static_assert(k_num_bits_per_sample % 8u == 0, "WAVE File Format Error: Bit depth must be multiple of 8");
    m_format->nChannels = k_num_channnels_per_sound;
    m_format->nSamplesPerSec = k_num_samples_per_sec;
    m_format->wBitsPerSample = k_num_bits_per_sample;
    m_format->nBlockAlign = (k_num_bits_per_sample / 8) * k_num_channnels_per_sound;
    m_format->nAvgBytesPerSec = m_format->nBlockAlign * k_num_samples_per_sec;
    m_format->cbSize = 0;
    m_format->wFormatTag = WAVE_FORMAT_PCM;

    // find address of DllGetClassObject() function in the dll
    const std::function<HRESULT(REFCLSID, REFIID, LPVOID)> DllGetClassObject =
        reinterpret_cast<HRESULT(WINAPI *)(REFCLSID, REFIID, LPVOID)>(
            GetProcAddress(m_xaudio_dll, "DllGetClassObject"));
    if (!DllGetClassObject) {
        throw CHILI_SOUND_API_EXCEPTION(
            HRESULT_FROM_WIN32(GetLastError()),
            L"Getting process address of 'DllGetClassObject' function");
    }

    // create the factory class for the XAudio2 component object
    Microsoft::WRL::ComPtr<IClassFactory> pClassFactory;
    HRESULT hr;
    if (FAILED(hr = DllGetClassObject(
        __uuidof(XAudio2),
        IID_IClassFactory,
        pClassFactory.ReleaseAndGetAddressOf())))
    {
        throw CHILI_SOUND_API_EXCEPTION(hr, L"Creating factory for XAudio2 object");
    }

    // create the XAudio2 component object itself
    if (FAILED(hr = pClassFactory->CreateInstance(nullptr,
        __uuidof(IXAudio2), &m_engine_ptr)))
    {
        throw CHILI_SOUND_API_EXCEPTION(hr, L"Creating XAudio2 object");
    }

    // initialize the XAudio2 component object
    if (FAILED(hr = m_engine_ptr->Initialize(0, XAUDIO2_DEFAULT_PROCESSOR)))
        throw CHILI_SOUND_API_EXCEPTION(hr, L"Initializing XAudio2 object");

    // create the mastering voice
    if (FAILED(hr = m_engine_ptr->CreateMasteringVoice(&m_master)))
        throw CHILI_SOUND_API_EXCEPTION(hr, L"Creating mastering voice");

    // create channel objects
    for (int i = 0; i < k_num_channels; i++)
        m_idle_channel_ptrs.push_back(std::make_unique<Channel>(*this));
}

void SoundSystem::deactivate_channel(Channel &channel)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    auto i = std::find_if(m_active_channel_ptrs.begin(), m_active_channel_ptrs.end(),
        [&channel](const std::unique_ptr<Channel> &pChan) -> bool
    {
        return &channel == pChan.get();
    });
    m_idle_channel_ptrs.push_back(std::move(*i));
    m_active_channel_ptrs.erase(i);
}

SoundSystem::Channel::Channel(SoundSystem &sys) : m_xa_buffer(std::make_unique<XAUDIO2_BUFFER>())
{
    class VoiceCallback : public IXAudio2VoiceCallback
    {
    public:
        void STDMETHODCALLTYPE OnStreamEnd() override { }
        void STDMETHODCALLTYPE OnVoiceProcessingPassEnd() override { }
        void STDMETHODCALLTYPE OnVoiceProcessingPassStart(UINT32 SamplesRequired) override { }
        void STDMETHODCALLTYPE OnBufferEnd(void *pBufferContext) override
        {
            Channel &chan = *reinterpret_cast<Channel *>(pBufferContext);
            chan.stop();
            {
                std::lock_guard<std::mutex> lock(chan.m_sound->m_mutex);
                chan.m_sound->m_active_channel_ptrs.erase(std::find(
                    chan.m_sound->m_active_channel_ptrs.begin(), chan.m_sound->m_active_channel_ptrs.end(), &chan));
                // notify any thread that might be waiting for activeChannels
                // to become zero (i.e. thread calling destructor)
                chan.m_sound->m_cv_death.notify_all();
            }
            chan.m_sound = nullptr;
            SoundSystem::get().deactivate_channel(chan);
        }
        void STDMETHODCALLTYPE OnBufferStart(void *pBufferContext) override { }
        void STDMETHODCALLTYPE OnLoopEnd(void *pBufferContext) override { }
        void STDMETHODCALLTYPE OnVoiceError(void *pBufferContext, HRESULT Error) override { }
    };
    static VoiceCallback vcb;
    ZeroMemory(m_xa_buffer.get(), sizeof(*m_xa_buffer));
    m_xa_buffer->pContext = this;
    HRESULT hr;
    if (FAILED(hr = sys.m_engine_ptr->CreateSourceVoice(&m_src, sys.m_format.get(), 0u, 2.0f, &vcb)))
        throw CHILI_SOUND_API_EXCEPTION(hr, L"Creating source voice for channel");
}

SoundSystem::Channel::~Channel()
{
    assert(!m_sound);
    if (m_src) {
        m_src->DestroyVoice();
        m_src = nullptr;
    }
}

void SoundSystem::Channel::play_sound_buffer(Sound &s, float freqMod, float vol)
{
    assert(m_src && !m_sound);
    {
        std::lock_guard<std::mutex> lock(s.m_mutex);
        s.m_active_channel_ptrs.push_back(this);
    }
    // callback thread not running yet, so no sync necessary for m_sound
    m_sound = &s;
    m_xa_buffer->pAudioData = s.m_data.get();
    m_xa_buffer->AudioBytes = s.m_num_bytes;
    if (s.m_looping) {
        m_xa_buffer->LoopBegin = s.m_loop_start;
        m_xa_buffer->LoopLength = s.m_loop_end - s.m_loop_start;
        m_xa_buffer->LoopCount = XAUDIO2_LOOP_INFINITE;
    } else {
        m_xa_buffer->LoopCount = 0u;
    }
    HRESULT hr;
    if (FAILED(hr = m_src->SubmitSourceBuffer(m_xa_buffer.get(), nullptr)))
        throw CHILI_SOUND_API_EXCEPTION(hr, L"Starting playback - submitting source buffer");
    if (FAILED(hr = m_src->SetFrequencyRatio(freqMod)))
        throw CHILI_SOUND_API_EXCEPTION(hr, L"Starting playback - setting frequency");
    if (FAILED(hr = m_src->SetVolume(vol)))
        throw CHILI_SOUND_API_EXCEPTION(hr, L"Starting playback - setting volume");
    if (FAILED(hr = m_src->Start()))
        throw CHILI_SOUND_API_EXCEPTION(hr, L"Starting playback - starting");
}

void SoundSystem::Channel::stop()
{
    assert(m_src && m_sound);
    m_src->Stop();
    m_src->FlushSourceBuffers();
}

void SoundSystem::Channel::retarget_sound(const Sound *pOld, Sound *pNew)
{
    assert(pOld == m_sound);
    m_sound = pNew;
}

Sound::Sound(const std::wstring &fileName, bool loopingWithAutoCueDetect) :
    Sound(fileName, loopingWithAutoCueDetect ?
        LoopType::AutoEmbeddedCuePoints : LoopType::NotLooping)
{ }

Sound::Sound(const std::wstring &fileName, LoopType loopType)
{
    if (fileName.substr(fileName.size() - 4u, 4u) == std::wstring{ L".wav" })
        *this = Sound(fileName, loopType, k_null_sample, k_null_sample, k_null_seconds, k_null_seconds);
    else
        *this = load_non_wav(fileName, loopType, k_null_sample, k_null_sample, k_null_seconds, k_null_seconds);
}

Sound::Sound(const std::wstring &fileName, unsigned int loopStart, unsigned int loopEnd)
{
    if (fileName.substr(fileName.size() - 4u, 4u) == std::wstring{ L".wav" })
        *this = Sound(fileName, LoopType::ManualSample, loopStart, loopEnd, k_null_seconds, k_null_seconds);
    else
        *this = load_non_wav(fileName, LoopType::ManualSample, loopStart, loopEnd, k_null_seconds, k_null_seconds);
}

Sound::Sound(const std::wstring &fileName, float loopStart, float loopEnd)
{
    if (fileName.substr(fileName.size() - 4u, 4u) == std::wstring{ L".wav" })
        *this = Sound(fileName, LoopType::ManualFloat, k_null_sample, k_null_sample, loopStart, loopEnd);
    else
        *this = load_non_wav(fileName, LoopType::ManualFloat, k_null_sample, k_null_sample, loopStart, loopEnd);
}

Sound Sound::load_non_wav(const std::wstring &fileName, LoopType loopType,
    unsigned int loopStartSample, unsigned int loopEndSample,
    float loopStartSeconds, float loopEndSeconds)
{
    namespace wrl = Microsoft::WRL;

    // if manual float looping, second inputs cannot be null
    assert((loopType == LoopType::ManualFloat) !=
        (loopStartSeconds == k_null_seconds || loopEndSeconds == k_null_seconds) &&
        "Did you pass a LoopType::Manual to the constructor? (BAD!)");
    // if manual sample looping, sample inputs cannot be null
    assert((loopType == LoopType::ManualSample) !=
        (loopStartSample == k_null_sample || loopEndSample == k_null_sample) &&
        "Did you pass a LoopType::Manual to the constructor? (BAD!)");
    // load from non-wav cannot use embedded loop points
    assert(loopType != LoopType::AutoEmbeddedCuePoints &&
        "load from non-wav cannot use embedded loop points");

    Sound sound;
    HRESULT hr;

    // make sure that the sound system is loaded first!
    SoundSystem::get();

    // creating source reader
    wrl::ComPtr<IMFSourceReader> pReader;
    if (FAILED(hr = MFCreateSourceReaderFromURL(fileName.c_str(), nullptr, &pReader)))
        throw CHILI_SOUND_API_EXCEPTION(hr, L"Creating MF Source Reader\nFilename: " + fileName);

    // selecting first stream
    if (FAILED(hr = pReader->SetStreamSelection(
        (DWORD)MF_SOURCE_READER_ALL_STREAMS, FALSE)))
    {
        throw CHILI_SOUND_API_EXCEPTION(hr, L"setting stream selection all");
    }

    if (FAILED(hr = pReader->SetStreamSelection(
        (DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, TRUE)))
    {
        throw CHILI_SOUND_API_EXCEPTION(hr, L"setting stream selection first");
    }

    // Configuring output format
    wrl::ComPtr<IMFMediaType> pUncompressedAudioType;
    {
        wrl::ComPtr<IMFMediaType> pPartialType;

        // configuring partial type
        if (FAILED(hr = MFCreateMediaType(&pPartialType)))
            throw CHILI_SOUND_API_EXCEPTION(hr, L"creating partial media type");

        if (FAILED(hr = pPartialType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio)))
            throw CHILI_SOUND_API_EXCEPTION(hr, L"setting partial media type major guid");

        if (FAILED(hr = pPartialType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM)))
            throw CHILI_SOUND_API_EXCEPTION(hr, L"setting partial media type sub guid");

        // Set this type on the source reader. The source reader will
        // load the necessary decoder.
        if (FAILED(hr = pReader->SetCurrentMediaType(
            (DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM,
            nullptr, pPartialType.Get())))
        {
            throw CHILI_SOUND_API_EXCEPTION(hr, L"setting partial type on source reader");
        }

        // Get the complete uncompressed format.
        if (FAILED(hr = pReader->GetCurrentMediaType(
            (DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM,
            &pUncompressedAudioType)))
        {
            throw CHILI_SOUND_API_EXCEPTION(hr, L"getting complete uncompressed format");
        }

        // Ensure the stream is selected.
        if (FAILED(hr = pReader->SetStreamSelection(
            (DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM,
            TRUE)))
        {
            throw CHILI_SOUND_API_EXCEPTION(hr, L"making sure stream is selected (who the fuck knows?)");
        }
    }

    // calculating number of sample bytes
    // and verifying that format matches sound system channels
    {
        UINT32 cbFormat = 0;

        // init smart pointer with custom deleter
        const auto pFormat = [&]()
        {
            // to be temp filled with the pointer
            WAVEFORMATEX *pFormat = nullptr;
            // loading format info into wave format structure (callee allocated, but we must free)
            if (FAILED(hr = MFCreateWaveFormatExFromMFMediaType(pUncompressedAudioType.Get(), &pFormat, &cbFormat)))
                throw CHILI_SOUND_API_EXCEPTION(hr, L"loading format info into wave format structure");
            return std::unique_ptr<WAVEFORMATEX, decltype(&CoTaskMemFree)>(pFormat, CoTaskMemFree);
        }();

        // compare format with sound system format
        {
            const WAVEFORMATEX &sysFormat = SoundSystem::get_format();

            if (pFormat->nChannels != sysFormat.nChannels)
                throw CHILI_SOUND_FILE_EXCEPTION(fileName, L"bad decompressed wave format (nChannels)");
            else if (pFormat->wBitsPerSample != sysFormat.wBitsPerSample)
                throw CHILI_SOUND_FILE_EXCEPTION(fileName, L"bad decompressed wave format (wBitsPerSample)");
            else if (pFormat->nSamplesPerSec != sysFormat.nSamplesPerSec)
                throw CHILI_SOUND_FILE_EXCEPTION(fileName, L"bad decompressed wave format (nSamplesPerSec)");
            else if (pFormat->wFormatTag != sysFormat.wFormatTag)
                throw CHILI_SOUND_FILE_EXCEPTION(fileName, L"bad decompressed wave format (wFormatTag)");
            else if (pFormat->nBlockAlign != sysFormat.nBlockAlign)
                throw CHILI_SOUND_FILE_EXCEPTION(fileName, L"bad decompressed wave format (nBlockAlign)");
            else if (pFormat->nAvgBytesPerSec != sysFormat.nAvgBytesPerSec)
                throw CHILI_SOUND_FILE_EXCEPTION(fileName, L"bad decompressed wave format (nAvgBytesPerSec)");
        }

        {
            // inheritance for automatic freeing of propvariant resources
            struct AutoPropVariant : PROPVARIANT
            {
                ~AutoPropVariant()
                {
                    PropVariantClear(this);
                }
            } var;

            // get duration attribute (as prop variant) from reader
            if (FAILED(hr = pReader->GetPresentationAttribute(MF_SOURCE_READER_MEDIASOURCE,
                MF_PD_DURATION, &var)))
            {
                throw CHILI_SOUND_API_EXCEPTION(hr, L"getting duration attribute from reader");
            }

            // getting int64 from duration prop variant
            long long duration;
            if (FAILED(hr = PropVariantToInt64(var, &duration)))
            {
                throw CHILI_SOUND_API_EXCEPTION(hr, L"getting int64 out of variant property (duration)");
            }

            // calculating number of bytes for samples (duration is in units of 100ns)
            // (adding extra 1 sec of padding for length calculation error margin)
            sound.m_num_bytes = UINT32((pFormat->nAvgBytesPerSec * duration) / 10000000 + pFormat->nAvgBytesPerSec);
        }
    }

    // allocate memory for sample data
    sound.m_data = std::make_unique<BYTE[]>(sound.m_num_bytes);

    // decode samples and copy into data buffer
    size_t nBytesWritten = 0u;
    while (true)
    {
        wrl::ComPtr<IMFSample> pSample;
        DWORD dwFlags = 0;

        // Read the next samples
        if (FAILED(hr = pReader->ReadSample(
            (DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM,
            0, nullptr, &dwFlags, nullptr, &pSample)))
        {
            throw CHILI_SOUND_API_EXCEPTION(hr, L"reading next samples");
        }

        if (dwFlags & MF_SOURCE_READERF_CURRENTMEDIATYPECHANGED)
            throw std::runtime_error("Type change - not supported by WAVE file format.@ ReadSample");

        if (dwFlags & MF_SOURCE_READERF_ENDOFSTREAM)
            break;

        if (pSample == nullptr)
            continue;

        // Get a pointer to the audio data in the sample.
        wrl::ComPtr<IMFMediaBuffer> pBuffer;
        if (FAILED(hr = pSample->ConvertToContiguousBuffer(&pBuffer)))
            throw CHILI_SOUND_API_EXCEPTION(hr, L"converting to contiguous buffer");

        BYTE *pAudioData = nullptr;
        DWORD cbBuffer = 0;
        if (FAILED(hr = pBuffer->Lock(&pAudioData, nullptr, &cbBuffer)))
            throw CHILI_SOUND_API_EXCEPTION(hr, L"locking sample buffer");

        // Make sure not to exceed the size of the buffer
        if (nBytesWritten + cbBuffer > sound.m_num_bytes)
            throw std::runtime_error("too many bytes being decoded wtf??!~");

        // copy the sample bytes
        memcpy(&sound.m_data[nBytesWritten], pAudioData, cbBuffer);

        // Update running total of audio data.
        nBytesWritten += cbBuffer;

        // Unlock the buffer.
        if (FAILED(hr = pBuffer->Unlock()))
            throw CHILI_SOUND_API_EXCEPTION(hr, L"unlocking sample buffer");
    }

    // reallocate buffer for proper size
    {
        auto pAdjustedBuffer = std::make_unique<BYTE[]>(nBytesWritten);
        // copy over bytes
        memcpy(pAdjustedBuffer.get(), sound.m_data.get(), nBytesWritten);
        // move buffer
        sound.m_data = std::move(pAdjustedBuffer);
        // adjust byte count
        sound.m_num_bytes = UINT32(nBytesWritten);
    }

    // setting looping parameters
    /////////////////////////////
    // setting looping parameters
    switch (loopType)
    {
        case LoopType::ManualFloat:
        {
            sound.m_looping = true;

            const WAVEFORMATEX &sysFormat = SoundSystem::get_format();
            const unsigned int nFrames = sound.m_num_bytes / sysFormat.nBlockAlign;

            const unsigned int nFramesPerSec = sysFormat.nAvgBytesPerSec / sysFormat.nBlockAlign;
            sound.m_loop_start = unsigned int(loopStartSeconds * float(nFramesPerSec));
            assert(sound.m_loop_start < nFrames);
            sound.m_loop_end = unsigned int(loopEndSeconds * float(nFramesPerSec));
            assert(sound.m_loop_end > sound.m_loop_start && sound.m_loop_end < nFrames);

            // just in case ;)
            sound.m_loop_start = std::min(sound.m_loop_start, nFrames - 1u);
            sound.m_loop_end = std::min(sound.m_loop_end, nFrames - 1u);
        }
        break;
        case LoopType::ManualSample:
        {
            sound.m_looping = true;

            const WAVEFORMATEX &sysFormat = SoundSystem::get_format();
            const unsigned int nFrames = sound.m_num_bytes / sysFormat.nBlockAlign;

            assert(loopStartSample < nFrames);
            sound.m_loop_start = loopStartSample;
            assert(loopEndSample > loopStartSample && loopEndSample < nFrames);
            sound.m_loop_end = loopEndSample;

            // just in case ;)
            sound.m_loop_start = std::min(sound.m_loop_start, nFrames - 1u);
            sound.m_loop_end = std::min(sound.m_loop_end, nFrames - 1u);
        }
        break;
        case LoopType::AutoFullSound:
        {
            sound.m_looping = true;

            const unsigned int nFrames = sound.m_num_bytes / SoundSystem::get_format().nBlockAlign;
            assert(nFrames != 0u && "Cannot auto full-loop on zero-length sound!");
            sound.m_loop_start = 0u;
            sound.m_loop_end = nFrames != 0u ? nFrames - 1u : 0u;
        }
        break;
        case LoopType::NotLooping:
            break;
        default:
            assert("Bad LoopType encountered!" && false);
            break;
    }
    /////////////////////////////

    return std::move(sound);
}

Sound::Sound(const std::wstring &fileName, LoopType loopType,
    unsigned int loopStartSample, unsigned int loopEndSample,
    float loopStartSeconds, float loopEndSeconds)
{
    // if manual float looping, second inputs cannot be null
    assert((loopType == LoopType::ManualFloat) !=
        (loopStartSeconds == k_null_seconds || loopEndSeconds == k_null_seconds) &&
        "Did you pass a LoopType::Manual to the constructor? (BAD!)");
    // if manual sample looping, sample inputs cannot be null
    assert((loopType == LoopType::ManualSample) !=
        (loopStartSample == k_null_sample || loopEndSample == k_null_sample) &&
        "Did you pass a LoopType::Manual to the constructor? (BAD!)");

    const auto IsFourCC = [](const BYTE *pData, const char *pFourcc)
    {
        assert(strlen(pFourcc) == 4);
        for (int i = 0; i < 4; i++) {
            if (char(pData[i]) != pFourcc[i])
                return false;
        }
        return true;
    };

    unsigned int fileSize = 0;
    std::unique_ptr<BYTE[]> pFileIn;
    try
    {
        {
            std::ifstream file;
            file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
            file.open(fileName, std::ios::binary);

            {
                BYTE fourcc[4];
                file.read(reinterpret_cast<char *>(fourcc), 4u);
                if (!IsFourCC(fourcc, "RIFF"))
                    throw CHILI_SOUND_FILE_EXCEPTION(fileName, L"Bad fourcc code");
            }

            file.read(reinterpret_cast<char *>(&fileSize), sizeof(fileSize));
            fileSize += 8u; // entry doesn't include the fourcc or itself
            if (fileSize <= 44u)
                throw CHILI_SOUND_FILE_EXCEPTION(fileName, L"file too small");

            file.seekg(0, std::ios::beg);
            pFileIn = std::make_unique<BYTE[]>(fileSize);
            file.read(reinterpret_cast<char *>(pFileIn.get()), fileSize);
        }

        if (!IsFourCC(&pFileIn[8], "WAVE"))
            throw CHILI_SOUND_FILE_EXCEPTION(fileName, L"format not WAVE");

        //look for 'fmt ' chunk id
        WAVEFORMATEX format;
        bool bFilledFormat = false;
        for (size_t i = 12u; i < fileSize; )
        {
            if (IsFourCC(&pFileIn[i], "fmt ")) {
                memcpy(&format, &pFileIn[i + 8u], sizeof(format));
                bFilledFormat = true;
                break;
            }
            // chunk size + size entry size + chunk id entry size + word padding
            unsigned int chunkSize;
            memcpy(&chunkSize, &pFileIn[i + 4u], sizeof(chunkSize));
            i += (chunkSize + 9u) & 0xFFFFFFFEu;
        }
        if (!bFilledFormat)
            throw CHILI_SOUND_FILE_EXCEPTION(fileName, L"fmt chunk not found");

        // compare format with sound system format
        {
            const WAVEFORMATEX &sysFormat = SoundSystem::get_format();

            if (format.nChannels != sysFormat.nChannels)
                throw CHILI_SOUND_FILE_EXCEPTION(fileName, L"bad wave format (nChannels)");
            else if (format.wBitsPerSample != sysFormat.wBitsPerSample)
                throw CHILI_SOUND_FILE_EXCEPTION(fileName, L"bad wave format (wBitsPerSample)");
            else if (format.nSamplesPerSec != sysFormat.nSamplesPerSec)
                throw CHILI_SOUND_FILE_EXCEPTION(fileName, L"bad wave format (nSamplesPerSec)");
            else if (format.wFormatTag != sysFormat.wFormatTag)
                throw CHILI_SOUND_FILE_EXCEPTION(fileName, L"bad wave format (wFormatTag)");
            else if (format.nBlockAlign != sysFormat.nBlockAlign)
                throw CHILI_SOUND_FILE_EXCEPTION(fileName, L"bad wave format (nBlockAlign)");
            else if (format.nAvgBytesPerSec != sysFormat.nAvgBytesPerSec)
                throw CHILI_SOUND_FILE_EXCEPTION(fileName, L"bad wave format (nAvgBytesPerSec)");
        }

        //look for 'data' chunk id
        bool bFilledData = false;
        for (size_t i = 12u; i < fileSize; )
        {
            unsigned int chunkSize;
            memcpy(&chunkSize, &pFileIn[i + 4u], sizeof(chunkSize));
            if (IsFourCC(&pFileIn[i], "data"))
            {
                m_data = std::make_unique<BYTE[]>(chunkSize);
                m_num_bytes = chunkSize;
                memcpy(m_data.get(), &pFileIn[i + 8u], m_num_bytes);

                bFilledData = true;
                break;
            }
            // chunk size + size entry size + chunk id entry size + word padding
            i += (chunkSize + 9u) & 0xFFFFFFFEu;
        }
        if (!bFilledData)
            throw CHILI_SOUND_FILE_EXCEPTION(fileName, L"data chunk not found");

        switch (loopType)
        {
            case LoopType::AutoEmbeddedCuePoints:
            {
                m_looping = true;

                //look for 'cue' chunk id
                bool bFilledCue = false;
                for (size_t i = 12u; i < fileSize; )
                {
                    unsigned int chunkSize;
                    memcpy(&chunkSize, &pFileIn[i + 4u], sizeof(chunkSize));
                    if (IsFourCC(&pFileIn[i], "cue "))
                    {
                        struct CuePoint
                        {
                            unsigned int cuePtId;
                            unsigned int pop;
                            unsigned int dataChunkId;
                            unsigned int chunkStart;
                            unsigned int blockStart;
                            unsigned int frameOffset;
                        };

                        unsigned int nCuePts;
                        memcpy(&nCuePts, &pFileIn[i + 8u], sizeof(nCuePts));
                        if (nCuePts == 2u)
                        {
                            CuePoint cuePts[2];
                            memcpy(cuePts, &pFileIn[i + 12u], sizeof(cuePts));
                            m_loop_start = cuePts[0].frameOffset;
                            m_loop_end = cuePts[1].frameOffset;
                            bFilledCue = true;
                            break;
                        }
                    }
                    // chunk size + size entry size + chunk id entry size + word padding
                    i += (chunkSize + 9u) & 0xFFFFFFFEu;
                }
                if (!bFilledCue)
                    throw CHILI_SOUND_FILE_EXCEPTION(fileName, L"loop cue chunk not found");
            }
            break;
            case LoopType::ManualFloat:
            {
                m_looping = true;

                const WAVEFORMATEX &sysFormat = SoundSystem::get_format();
                const unsigned int nFrames = m_num_bytes / sysFormat.nBlockAlign;

                const unsigned int nFramesPerSec = sysFormat.nAvgBytesPerSec / sysFormat.nBlockAlign;
                m_loop_start = unsigned int(loopStartSeconds * float(nFramesPerSec));
                assert(m_loop_start < nFrames);
                m_loop_end = unsigned int(loopEndSeconds * float(nFramesPerSec));
                assert(m_loop_end > m_loop_start && m_loop_end < nFrames);

                // just in case ;)
                m_loop_start = std::min(m_loop_start, nFrames - 1u);
                m_loop_end = std::min(m_loop_end, nFrames - 1u);
            }
            break;
            case LoopType::ManualSample:
            {
                m_looping = true;

                const WAVEFORMATEX &sysFormat = SoundSystem::get_format();
                const unsigned int nFrames = m_num_bytes / sysFormat.nBlockAlign;

                assert(loopStartSample < nFrames);
                m_loop_start = loopStartSample;
                assert(loopEndSample > loopStartSample && loopEndSample < nFrames);
                m_loop_end = loopEndSample;

                // just in case ;)
                m_loop_start = std::min(m_loop_start, nFrames - 1u);
                m_loop_end = std::min(m_loop_end, nFrames - 1u);
            }
            break;
            case LoopType::AutoFullSound:
            {
                m_looping = true;

                const unsigned int nFrames = m_num_bytes / SoundSystem::get_format().nBlockAlign;
                assert(nFrames != 0u && "Cannot auto full-loop on zero-length sound!");
                m_loop_start = 0u;
                m_loop_end = nFrames != 0u ? nFrames - 1u : 0u;
            }
            break;
            case LoopType::NotLooping:
                break;
            default:
                assert("Bad LoopType encountered!" && false);
                break;
        }
    }
    catch (const SoundSystem::FileException &e)
    {
        m_num_bytes = 0u;
        m_looping = false;
        m_data.release();
        throw e;
    }
    catch (const std::exception &e)
    {
        m_num_bytes = 0u;
        m_looping = false;
        m_data.release();
        // needed for conversion to wide string
        const std::string what = e.what();
        throw CHILI_SOUND_FILE_EXCEPTION(fileName, std::wstring(what.begin(), what.end()));
    }
}

Sound::Sound(Sound &&donor) noexcept
{
    std::lock_guard<std::mutex> lock(donor.m_mutex);
    m_num_bytes = donor.m_num_bytes;
    donor.m_num_bytes = 0u;
    m_looping = donor.m_looping;
    m_loop_start = donor.m_loop_start;
    m_loop_end = donor.m_loop_end;
    m_data = std::move(donor.m_data);
    m_active_channel_ptrs = std::move(donor.m_active_channel_ptrs);
    for (auto &pChan : m_active_channel_ptrs)
        pChan->retarget_sound(&donor, this);
    donor.m_cv_death.notify_all();
}

Sound &Sound::operator=(Sound &&donor) noexcept
{
    // make sure nobody messes with our shit (also needed for cv.wait())
    std::unique_lock<std::mutex> lock(m_mutex);
    // check if there are even any active channels playing our jam
    if (m_active_channel_ptrs.size() != 0u)
    {
        // stop all channels currently playing our jam
        for (auto pChannel : m_active_channel_ptrs)
            pChannel->stop();
        // wait for those channels to actually stop playing our jam
        m_cv_death.wait(lock, [this] { return m_active_channel_ptrs.size() == 0u; });
    }

    std::lock_guard<std::mutex> lock_donor(donor.m_mutex);
    m_num_bytes = donor.m_num_bytes;
    donor.m_num_bytes = 0u;
    m_looping = donor.m_looping;
    m_loop_start = donor.m_loop_start;
    m_loop_end = donor.m_loop_end;
    m_data = std::move(donor.m_data);
    m_active_channel_ptrs = std::move(donor.m_active_channel_ptrs);
    for (auto &pChan : m_active_channel_ptrs)
        pChan->retarget_sound(&donor, this);
    donor.m_cv_death.notify_all();
    return *this;
}

void Sound::play(float freqMod, float vol)
{
    SoundSystem::get().play_sound_buffer(*this, freqMod, vol);
}

void Sound::stop_one()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_active_channel_ptrs.size() > 0u)
        m_active_channel_ptrs.front()->stop();
}

void Sound::stop_all()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    for (auto pChannel : m_active_channel_ptrs)
        pChannel->stop();
}

Sound::~Sound()
{
    // make sure nobody messes with our shit (also needed for cv.wait())
    std::unique_lock<std::mutex> lock(m_mutex);

    // check if there are even any active channels playing our jam
    if (m_active_channel_ptrs.size() == 0u)
        return;

    // stop all channels currently playing our jam
    for (auto pChannel : m_active_channel_ptrs)
        pChannel->stop();

    // wait for those channels to actually stop playing our jam
    m_cv_death.wait(lock, [this] { return m_active_channel_ptrs.size() == 0u; });
}

SoundSystem::APIException::APIException(HRESULT hr, const wchar_t *file, unsigned int line, const std::wstring &note)
    :
    m_hr(hr), ChiliException(file, line, note)
{ }

std::wstring SoundSystem::APIException::get_full_message() const
{
    return L"Error Name: " + get_error_name() + L"\n\n" +
        L"Error Description: " + get_error_description() + L"\n\n" +
        L"Note: " + get_note() + L"\n\n" +
        L"Location: " + get_location();
}

std::wstring SoundSystem::APIException::get_exception_type() const
{
    return L"Sound System API Exception";
}

std::wstring SoundSystem::APIException::get_error_name() const
{
    return DXGetErrorString(m_hr);
}

std::wstring SoundSystem::APIException::get_error_description() const
{
    std::array<wchar_t, 512> wideDescription;
    DXGetErrorDescription(m_hr, wideDescription.data(), wideDescription.size());
    return wideDescription.data();
}

SoundSystem::FileException::FileException(const wchar_t *file, unsigned int line, const std::wstring &note, const std::wstring &filename)
    :
    ChiliException(file, line, note), m_filename(filename)
{ }

std::wstring SoundSystem::FileException::get_full_message() const
{
    return L"Filename: " + m_filename + L"\n\n" +
        L"Note: " + get_note() + L"\n\n" +
        L"Location: " + get_location();
}

std::wstring SoundSystem::FileException::get_exception_type() const
{
    return L"Sound System File Exception";
}

SoundSystem::MFInitializer::MFInitializer()
{
    m_hr = MFStartup(MF_VERSION);
}

SoundSystem::MFInitializer::~MFInitializer()
{
    if (m_hr == S_OK)
    {
        MFShutdown();
    }
}
