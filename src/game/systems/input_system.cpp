#include "input_system.h"
#include "comps.h"
#include "camera.h"
#include "input.h"

KeyState
next_state(KeyState current, bool pressed)
{
  if (!pressed) {
    return KeyState::Release;
  }
  if (current == KeyState::JustPressed) {
    return KeyState::Hold;
  }
  return KeyState::JustPressed;
}

void
input_update(Input& ic)
{
  ic.mouse = WInput::get_mouse_pos();
  ic.left = next_state(ic.left, WInput::is_key_pressed(GLFW_KEY_A));
  ic.right = next_state(ic.right, WInput::is_key_pressed(GLFW_KEY_D));
}
