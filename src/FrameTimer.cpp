//
// Created by jammer on 20/04/17.
//

#include "FrameTimer.h"
#include <GLFW/glfw3.h>

void
FrameTimer::reset()
{
  glfwSetTime(0);
  m_cont_timer = 0;
}

void
FrameTimer::update()
{
  auto now = glfwGetTime();
  m_elapsed = now - m_cont_timer;
  m_cont_timer = now;
  m_fixed_counter += m_elapsed;
}

bool
FrameTimer::pop_fixed()
{
  if (m_fixed_counter >= c_fixed_timestep) {
    m_fixed_counter -= c_fixed_timestep;
    return true;
  }
  return false;
}
