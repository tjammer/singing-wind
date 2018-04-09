#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include "imgui_impl_glfw_gl3.h"
#include "input.h"
#include "col_shape.h"
#include "renderer.h"
#include "camera.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_transform_2d.hpp>
#include "ecs.hpp"

#include "comps.h"

using ecs::tag;

struct PosComp
{
  WTransform tf;
  ColCircle shape;
};

struct VelComp
{};

using Tag = tag<struct SF>;

void
pos_update(PosComp& pc, ecs::World& world, ecs::World::ent_id id)
{
  pc.tf = glm::translate(WTransform(), WInput::get_mouse_pos());
  WRenderer::set_mode(PLines);
  pc.shape.add_gfx_lines(pc.tf);
  // world.create_component(id, Tag{});
}

void vel_update(Tag)
{
  assert(false);
}

void
render_update()
{
  WRenderer::render_array();
}

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

  Camera camera{ viewport };

  WRenderer::init(window);
  WRenderer::set_camera(camera.get_camera());

  // these get overridden by imui, will get called there too
  glfwSetKeyCallback(window, WInput::key_callback);
  glfwSetMouseButtonCallback(window, WInput::mouse_button_callback);
  glfwSetScrollCallback(window, WInput::scroll_callback);
  glfwSetCharCallback(window, WInput::char_callback);

  ecs::World world;

  auto player = world.create_entity();
  world.create_component<PosComp>(player, { WTransform(), ColCircle(10) });

  while (glfwWindowShouldClose(window) == 0) {
    glfwPollEvents();
    ImGui_ImplGlfwGL3_NewFrame();
    glClearColor(0.16f, 0.19f, 0.23f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    double cursor[2];
    glfwGetCursorPos(window, &cursor[0], &cursor[1]);
    auto unpr_cursor = camera.unproject_mouse(cursor);
    WInput::set_mouse(unpr_cursor);

    WRenderer::reset();
    world.visit(pos_update);
    world.visit(vel_update);
    render_update();

    ImGui::Text("%f, %f", unpr_cursor.x, unpr_cursor.y);

    ImGui::Render();
    glfwSwapBuffers(window);
  }

  ImGui_ImplGlfwGL3_Shutdown();
  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}
