//
// Created by jammer on 16/05/17.
//

#include "WInput.h"
#include <unordered_map>

namespace WInput {
    std::unordered_map<int, int> keys;
    std::unordered_map<int, int> buttons;

    void key_callback(GLFWwindow*, int key, int, int action, int) {
        keys[key] = action;
    }

    bool is_key_pressed(int key) {
        return keys[key] == GLFW_PRESS || keys[key] == GLFW_REPEAT;
    }

    void mouse_button_callback(GLFWwindow*, int button, int action, int) {
        buttons[button] = action;
    }

    bool is_mouse_button_pressed(int button) {
        return buttons[button] == GLFW_PRESS;
    }
}