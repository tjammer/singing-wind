//
// Created by jammer on 16/05/17.
//

#ifndef SINGING_WIND_WINPUT_H
#define SINGING_WIND_WINPUT_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace WInput {
    void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    bool is_key_pressed(int key);

    void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    bool is_mouse_button_pressed(int button);
}


#endif //SINGING_WIND_WINPUT_H
