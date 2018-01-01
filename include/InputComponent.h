#ifndef CINPUT_H
#define CINPUT_H

#include "WindDefs.h"
#include "input_buffer.h"
#include "SkillComponent.h"

#include <map>
#include <array>

class GameWorld;

const int c_input_buffer_length = 15;

enum class InputFunc : int
{
  Protagonist,
  AI, // needs AI component to work
  state_count
};
const std::map<InputFunc, const char*> inputfunc_string = {
  { InputFunc::Protagonist, "Protagonist" },
  { InputFunc::AI, "AI" }
};

struct InputComponent
{
  input_buffer<int, c_input_buffer_length> direction;
  input_buffer<bool, c_input_buffer_length> jump;
  input_buffer<WVec, c_input_buffer_length> mouse;
  input_buffer<bool, c_input_buffer_length> wings;
  std::array<input_buffer<bool, c_input_buffer_length>, c_skill_slots> attacks;
  InputFunc input_func;
};

#endif /* CINPUT_H */
