#ifndef WINDOWS_AUDIO
#define WINDOWS_AUDIO
#include "pch.hpp"
#ifdef WINDOWS
#include <windows.h>
#include <mmsystem.h>
#include <Mmreg.h>


class WindowsAudio
{
public:
    static const int numberOfAudioBuffers = 5;
    static const int audioBufferSize = 4096;
    static WindowsAudio* audio;
    std::vector<int16_t> samples;
    WindowsAudio(HWND handle, int sampleRate);
    ~WindowsAudio();
    bool play();
    bool stop();
    CHAR* getFaultReason();
    virtual bool onDataArrival(std::vector<int16_t>& data) = 0;

protected:
    static void CALLBACK Callback(HWAVEOUT hWaveOut, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);
    void submitBuffer();

    HWAVEOUT      hwaveOut;
    HANDLE        handle;
    WAVEFORMATEX  format;
    WAVEHDR       waveOutHdr[numberOfAudioBuffers];
    int16_t       waveOut[numberOfAudioBuffers][audioBufferSize];
    int           waveOutPosition;
    int           sampleRate;
    bool          isReleasing;
    MMRESULT result;
    CHAR fault[256];
};

#endif
#endif