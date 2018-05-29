//
// Created by jammer on 16/05/17.
//

#include "input.h"
#include <imgui.h>
#include "imgui_impl_glfw_gl3.h"
#include <unordered_map>

namespace WInput {
std::unordered_map<int, int> keys;
std::unordered_map<int, int> buttons;
WVec mouse;
double raw_mouse[2];

float scroll_offset{ 0 };
float scroll_accum{ 0 };

void
key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  keys[key] = action;
  ImGui_ImplGlfwGL3_KeyCallback(window, key, scancode, action, mods);
}

bool
is_key_pressed(int key)
{
  return keys[key] == GLFW_PRESS || keys[key] == GLFW_REPEAT;
}

void
mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
  buttons[button] = action;
  ImGui_ImplGlfwGL3_MouseButtonCallback(window, button, action, mods);
}

bool
is_mouse_button_pressed(int button)
{
  return buttons[button] == GLFW_PRESS;
}

void
cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
  raw_mouse[0] = xpos;
  raw_mouse[1] = ypos;
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

double*
get_raw_mouse()
{
  return raw_mouse;
}

void
scroll_callback(GLFWwindow* window, double dx, double dy)
{
  scroll_accum += static_cast<float>(dy);
  ImGui_ImplGlfwGL3_ScrollCallback(window, dx, dy);
}

void
char_callback(GLFWwindow* window, unsigned int codepoint)
{
  ImGui_ImplGlfwGL3_CharCallback(window, codepoint);
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
} // WInput
