//
// Created by jammer on 16/05/17.
//

#ifndef SINGING_WIND_WINPUT_H
#define SINGING_WIND_WINPUT_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "wind_defs.h"

namespace WInput {
void
key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
bool
is_key_pressed(int key);

void
mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
bool
is_mouse_button_pressed(int button);

void
cursor_position_callback(GLFWwindow* window, double xpos, double ypos);

void
set_mouse(const WVec& mouse);
WVec
get_mouse_pos();
double*
get_raw_mouse();

void
scroll_callback(GLFWwindow*, double dx, double dy);
float
get_scroll_diff();
void
fix_scroll_for_frame();

void
char_callback(GLFWwindow* window, unsigned int);
} // WInput
#endif // SINGING_WIND_WINPUT_H
