#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include "imgui_impl_glfw_gl3.h"
#include "Engine.h"
#include <WInput.h>
#include <iostream>

int main() {
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(1280, 720, "My Title", NULL, NULL);

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    glfwSwapInterval(0);

    // these get overridden by imui, will get called there too
    glfwSetKeyCallback(window, WInput::key_callback);
    glfwSetMouseButtonCallback(window, WInput::mouse_button_callback);

    Engine engine(*window);

    ImGui_ImplGlfwGL3_Init(window, true);

    double time = 0;
    std::vector<double> means;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        ImGui_ImplGlfwGL3_NewFrame();
        engine.update();

        ImGui::Render();
        glfwSwapBuffers(window);
        auto now = glfwGetTime();
        means.push_back(now - time);
        time = now;
        if (means.size() > 10000) {
            break;
        }
    }
    double mean = 0;
    for (auto m : means) {
        mean += m;
    }
    std::cout << mean / means.size() << std::endl;

    ImGui_ImplGlfwGL3_Shutdown();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}