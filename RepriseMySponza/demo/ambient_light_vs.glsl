#version 330

layout (location = 0) in vec2 vertex_position;

void main(void)
{
    gl_Position = vec4(vertex_position, 0.0, 1.0);
}
