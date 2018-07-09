#version 330 core

layout (location = 0) in vec2 position;
layout (location = 1) in vec2 tex;

out vec2 TexCoord;

uniform mat4 transform;

void main()
{
    gl_Position = transform * vec4(position, 0.0, 1.0);
    TexCoord = vec2(tex.x, -tex.y);
}
