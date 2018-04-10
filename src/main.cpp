#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include "imgui_impl_glfw_gl3.h"
#include "input.h"
#include "renderer.h"
#include "game.h"

int
main()
{
  glfwInit();

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_SAMPLES, 2);

  WVec viewport{ 1280, 720 };
  GLFWwindow* window =
    glfwCreateWindow(viewport.x, viewport.y, "singing_wind", nullptr, nullptr);

  glfwMakeContextCurrent(window);
  gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
  glfwSwapInterval(1);

  ImGui_ImplGlfwGL3_Init(window, false);

  WRenderer::init(window);

  Game game{ viewport };

  glfwSetKeyCallback(window, WInput::key_callback);
  glfwSetMouseButtonCallback(window, WInput::mouse_button_callback);
  glfwSetScrollCallback(window, WInput::scroll_callback);
  glfwSetCharCallback(window, WInput::char_callback);
  glfwSetCursorPosCallback(window, WInput::cursor_position_callback);

  while (glfwWindowShouldClose(window) == 0) {
    glfwPollEvents();
    ImGui_ImplGlfwGL3_NewFrame();
    glClearColor(0.16f, 0.19f, 0.23f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    WRenderer::reset();
    game.update();
    game.draw();

    ImGui::Render();
    glfwSwapBuffers(window);
  }

  ImGui_ImplGlfwGL3_Shutdown();
  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}
