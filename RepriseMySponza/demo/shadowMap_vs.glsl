#version 330

layout (std140) uniform PerFrameUniforms
{
	mat4 projectionViewXform;
	vec3 cameraPos;
};

layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_normal;
layout (location = 3) in float mat_colour;
layout (location = 4) in mat4 model_Xform;

uniform mat4 lightPV;

void main(void)
{
    gl_Position = lightPV * model_Xform * vec4(vertex_position, 1);
}
