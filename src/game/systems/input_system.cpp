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
  ic.wings = next_state(
    ic.wings, WInput::is_mouse_button_pressed(GLFW_MOUSE_BUTTON_RIGHT));
}
