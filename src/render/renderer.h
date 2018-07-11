//
// Created by jammer on 16/05/17.
//

#ifndef GLFW33_WRENDERER_H
#define GLFW33_WRENDERER_H

#include <vector>
#include <glad/glad.h>
// get rid of this later
#include <glm/fwd.hpp>
#include <GLFW/glfw3.h>
#include "w_vec.h"

struct Camera;
struct Transform;

struct PrimitiveVertex
{
  float position[2];
  float color[3];
};

struct TexturedVertex
{
  float position[2];
  float tex_coord[2];
};

enum PrimitiveStyle
{
  PLines,
  PQuads,
  PTextured
};

namespace WRenderer {
void
init(GLFWwindow* window);
void
shutdown();
void
reset();
void
add_primitive_vertex(const PrimitiveVertex& vert);
void
add_sprite(const Transform& t, const WVec& size, unsigned int tex);
void
render_array();
void
set_mode(int mode);
void
set_camera(const Camera& camera, bool force = false);
void
resize(GLFWwindow*, int width, int height);
WVec
unproject_mouse(double pos[2]);
}

#endif // GLFW33_WRENDERER_H
