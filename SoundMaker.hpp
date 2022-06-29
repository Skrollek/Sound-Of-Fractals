#ifndef SOUNDMAKER
#define SOUNDMAKER
#include "pch.hpp"
#include "Engine.hpp"

#ifdef WINDOWS
#include "WindowsAudio.hpp"

class SoundMaker : public WindowsAudio
{
public:
  bool audio_reset;
  bool audio_pause;

  bool normalized;

  bool sustain;

  double volume;
  double play_x, play_y;
  double play_cx, play_cy;
  double play_nx, play_ny;
  double play_px, play_py;

  SoundMaker (HWND hwnd);

  void SetPoint(double x, double y);

  bool onDataArrival(std::vector<int16_t>& data) override;

  int16_t m_samples[audioBufferSize];
  int m_audio_time;
  double mean_x;
  double mean_y;
  double dx;
  double dy;
  double dpx;
  double dpy;
};


#endif

#endif