#version 330


layout (std140) uniform PerFrameUniforms
{
	mat4 projectionViewXform;
	vec3 cameraPos;
};

in vec3 varying_position;
in vec3 varying_normal;
in vec2 varying_texCoords;
in float varying_mat_colour;

layout (location = 0) out vec3 fragment_position;
layout (location = 1) out vec2 fragment_normal;
layout (location = 2) out float fragment_matColour;

void main(void)
{
	fragment_position = varying_position;
	fragment_normal = varying_normal.xy;
	fragment_matColour = varying_mat_colour;
}
