#include "SoundMaker.hpp"

SoundMaker::SoundMaker (HWND hwnd) : WindowsAudio(hwnd, SAMPLE_RATE) 
{
    audio_reset = true;
    audio_pause = false;
    volume = 8000.0;
    play_x = 0.0;
    play_y = 0.0;
    play_cx = 0.0;
    play_cy = 0.0;
    play_nx = 0.0;
    play_ny = 0.0;
    play_px = 0.0;
    play_py = 0.0;
}

void SoundMaker::SetPoint(double x, double y) 
{
    play_nx = x;
    play_ny = y;
    audio_reset = true;
    audio_pause = false;
}

bool SoundMaker::onDataArrival(std::vector<int16_t>& data)
{
    data.resize(audioBufferSize);
    memcpy(data.data(), m_samples, sizeof(int16_t) * audioBufferSize);
    memset(m_samples, 0, sizeof(int16_t) * audioBufferSize);

    //Check if audio needs to reset
    if (audio_reset) {
      m_audio_time = 0;
      play_cx = play_nx;
      play_cy = play_ny;
      play_x = play_nx;
      play_y = play_ny;
      play_px = play_nx;
      play_py = play_ny;
      mean_x = play_nx;
      mean_y = play_ny;
      volume = 8000.0;
      audio_reset = false;
    }

    //Check if paused
    if (audio_pause) {
      return true;
    }

    //Generate the tones
    const int steps = sampleRate / MAX_FREQ;
    for (int i = 0; i < audioBufferSize; i+=2) 
    {
      const int j = m_audio_time % steps;
      if (j == 0) 
      {
        play_px = play_x;
        play_py = play_y;
        auto nextPoint = Engine::getInstance()->getCurrentFractal()->nextPoint(std::complex<double>(play_x, play_y), std::complex<double>(play_cx, play_cy));
        play_x = nextPoint.real();
        play_y = nextPoint.imag();
        if (play_x*play_x + play_y*play_y > MAX_POINT_DISTANCE_FROM_ORIGIN) 
        {
          audio_pause = true;
          return true;
        }

        if (normalized) 
        {
          dpx = play_px - play_cx;
          dpy = play_py - play_cy;
          dx = play_x - play_cx;
          dy = play_y - play_cy;
          if (dx != 0.0 || dy != 0.0) 
          {
            double dpmag = 1.0 / std::sqrt(1e-12 + dpx*dpx + dpy*dpy);
            double dmag = 1.0 / std::sqrt(1e-12 + dx*dx + dy*dy);
            dpx *= dpmag;
            dpy *= dpmag;
            dx *= dmag;
            dy *= dmag;
          }
        } else 
        {
          //Point is relative to mean
          dx = play_x - mean_x;
          dy = play_y - mean_y;
          dpx = play_px - mean_x;
          dpy = play_py - mean_y;
        }

        //Update mean
        mean_x = mean_x*0.99 + play_x*0.01;
        mean_y = mean_y*0.99 + play_y*0.01;

        //Don't let the volume go to infinity, clamp.
        double m = dx*dx + dy*dy;
        if (m > 2.0) 
        {
          dx *= 2.0 / m;
          dy *= 2.0 / m;
        }
        m = dpx*dpx + dpy*dpy;
        if (m > 2.0) 
        {
          dpx *= 2.0 / m;
          dpy *= 2.0 / m;
        }

        //Lose volume over time unless in sustain mode
        if (!sustain) 
        {
          volume *= 0.9992;
        }
      }

      //Cosine interpolation
      double t = double(j) / double(steps);
      t = 0.5 - 0.5*std::cos(t * 3.14159);
      double wx = t*dx + (1.0 - t)*dpx;
      double wy = t*dy + (1.0 - t)*dpy;

      //Save the audio to the 2 channels
      m_samples[i]   = (int16_t)std::min(std::max(wx * volume, -32000.0), 32000.0);
      m_samples[i+1] = (int16_t)std::min(std::max(wy * volume, -32000.0), 32000.0);
      m_audio_time += 1;
    }
    memcpy(data.data(), m_samples, sizeof(int16_t) * audioBufferSize);
    //Return the sound clip
    return !audio_reset;
}