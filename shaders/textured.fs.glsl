#version 330 core
out vec4 color;

in vec2 tex_coord;

uniform sampler2D our_tex;

void main()
{
    color = texture(our_tex, tex_coord);
}
