#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include "imgui_impl_glfw_gl3.h"
#include "input.h"
#include "col_shape.h"
#include "renderer.h"

int
main()
{
  glfwInit();

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_SAMPLES, 2);
  GLFWwindow* window =
    glfwCreateWindow(1280, 720, "singing_wind", nullptr, nullptr);

  glfwMakeContextCurrent(window);
  gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
  glfwSwapInterval(1);

  ImGui_ImplGlfwGL3_Init(window, false);

  WRenderer::init(window);

  // these get overridden by imui, will get called there too
  glfwSetKeyCallback(window, WInput::key_callback);
  glfwSetMouseButtonCallback(window, WInput::mouse_button_callback);
  glfwSetScrollCallback(window, WInput::scroll_callback);
  glfwSetCharCallback(window, WInput::char_callback);

  auto pos = WVec(0, 0);
  auto shape = ColCircle(10);

  while (glfwWindowShouldClose(window) == 0) {
    glfwPollEvents();
    ImGui_ImplGlfwGL3_NewFrame();
    glClearColor(0.16f, 0.19f, 0.23f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    // engine.update();
    ImGui::Text("Hello, world!");

    ImGui::Render();
    glfwSwapBuffers(window);
  }

  ImGui_ImplGlfwGL3_Shutdown();
  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}
