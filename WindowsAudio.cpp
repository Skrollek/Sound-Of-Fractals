#include "WindowsAudio.hpp"

WindowsAudio* WindowsAudio::audio;

WindowsAudio::WindowsAudio(HWND handle, int sampleRate)
{
    waveOutPosition = 0;

    this->sampleRate = SAMPLE_RATE;
    isReleasing = false;

    format.wFormatTag = WAVE_FORMAT_PCM;
    format.nChannels = 2;
    format.nSamplesPerSec = sampleRate;
    format.nAvgBytesPerSec = sampleRate * 2;
    format.nBlockAlign = 4;
    format.wBitsPerSample = 16;
    format.cbSize = 0;

    audio = this;
}

WindowsAudio::~WindowsAudio() {
  stop();
}

bool WindowsAudio::play()
{
    handle = CreateMutex(NULL, FALSE, NULL);
    if(handle == NULL)
        return false;
    isReleasing = false;
    result = waveOutOpen(&hwaveOut, WAVE_MAPPER, &format, (DWORD_PTR)Callback, (DWORD_PTR)NULL, CALLBACK_FUNCTION);
    if(result != MMSYSERR_NOERROR)
    {
        waveInGetErrorText(result, fault, 256);
        return false;
    }

    for(int i = 0; i < numberOfAudioBuffers; ++i)
    {
        memset(waveOut[i], 0, audioBufferSize * sizeof(int16_t));
        memset(&waveOutHdr[i], 0, sizeof(WAVEHDR));
        waveOutHdr[i].lpData = (LPSTR)waveOut[i];
        waveOutHdr[i].dwBufferLength = audioBufferSize * sizeof(int16_t);
        result = waveOutPrepareHeader(hwaveOut, &waveOutHdr[i], sizeof(WAVEHDR));
        if(result != MMSYSERR_NOERROR)
        {
            waveInGetErrorText(result, fault, 256);
        }

    }

    submitBuffer();
    submitBuffer();
    return true;
}
bool WindowsAudio::stop()
{
    if (isReleasing) 
    {
        return true;
    }
    WaitForSingleObject(handle, INFINITE);
    isReleasing = true;
    for (int i = 0; i < numberOfAudioBuffers; i++) {
        result = waveOutReset(hwaveOut);
        if (result != MMSYSERR_NOERROR) 
        {
            waveInGetErrorText(result, fault, 256);
            return false;
        }
        result = waveOutUnprepareHeader(hwaveOut, &waveOutHdr[i], sizeof(WAVEHDR));
        if (result != MMSYSERR_NOERROR) 
        {
            waveInGetErrorText(result, fault, 256);
            return false;
        }
  }
  waveOutClose(hwaveOut);
  ReleaseMutex(handle);
  return true;
}

void WindowsAudio::submitBuffer()
{
    if (isReleasing)
    { 
        return; 
    }
    WaitForSingleObject(handle, INFINITE);
    result = waveOutWrite(hwaveOut, &waveOutHdr[waveOutPosition], sizeof(WAVEHDR));

    if (result != MMSYSERR_NOERROR) {
        waveInGetErrorText(result, fault, 256);
    }
    waveOutPosition = (waveOutPosition + 1) % numberOfAudioBuffers;
    samples.clear();
    onDataArrival(samples);
    memcpy(waveOut[waveOutPosition], samples.data(), samples.size() * sizeof(int16_t));
    ReleaseMutex(handle);
}

CHAR* WindowsAudio::getFaultReason()
{
    return fault;
}

void CALLBACK WindowsAudio::Callback(HWAVEOUT hWaveOut, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
    if(uMsg != WOM_DONE) return;
    audio->submitBuffer();
}

