//
// Created by jammer on 16/05/17.
//

#include "WInput.h"
#include <unordered_map>

namespace WInput {
std::unordered_map<int, int> keys;
std::unordered_map<int, int> buttons;
WVec mouse;

float scroll_offset{ 0 };
float scroll_accum{ 0 };

void
key_callback(GLFWwindow*, int key, int, int action, int)
{
  keys[key] = action;
}

bool
is_key_pressed(int key)
{
  return keys[key] == GLFW_PRESS || keys[key] == GLFW_REPEAT;
}

void
mouse_button_callback(GLFWwindow*, int button, int action, int)
{
  buttons[button] = action;
}

bool
is_mouse_button_pressed(int button)
{
  return buttons[button] == GLFW_PRESS;
}

void
set_mouse(const WVec& _mouse)
{
  mouse = _mouse;
}

WVec
get_mouse_pos()
{
  return mouse;
}

void
scroll_callback(GLFWwindow*, double, double dy)
{
  scroll_accum += static_cast<float>(dy);
}

float
get_scroll_diff()
{
  return scroll_offset;
}

void
fix_scroll_for_frame()
{
  scroll_offset = scroll_accum;
  scroll_accum = 0;
}
}
