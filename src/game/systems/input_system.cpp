#include "input_system.h"
#include "comps.h"
#include "camera.h"
#include "input.h"

void
input_update(Input& ic)
{
  ic.mouse = WInput::get_mouse_pos();
  ic.left_click.push(
    (KeyState)WInput::is_mouse_button_pressed(GLFW_MOUSE_BUTTON_LEFT));
  ic.left.push((KeyState)WInput::is_key_pressed(GLFW_KEY_A));
  ic.right.push((KeyState)WInput::is_key_pressed(GLFW_KEY_D));
  ic.jump.push(
    (KeyState)WInput::is_mouse_button_pressed(GLFW_MOUSE_BUTTON_RIGHT));
}
