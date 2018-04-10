//
// Created by jammer on 20/04/17.
//

#ifndef SINGING_WIND_FRAMETIMER_H
#define SINGING_WIND_FRAMETIMER_H

const float FIXED_TIMESTEP = 1.f / 60.f;

class FrameTimer
{
public:
  void update();
  void reset();
  bool pop_fixed();
  double get_elapsed() const { return m_elapsed; }
  float get_remaining() const { return m_fixed_counter; }

private:
  double m_elapsed;
  float m_fixed_counter = 0;
  double m_cont_timer = 0;
};

#endif // SINGING_WIND_FRAMETIMER_H
