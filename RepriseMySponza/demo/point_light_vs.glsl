#version 330



layout (std140) uniform PerFrameUniforms
{
	mat4 projectionViewXform;
	vec3 ambientColour;
	float pad;
	vec3 cameraPos;
};


uniform mat4 modelXform;

layout (location = 0) in vec3 vertex_position;

void main(void)
{
    gl_Position = projectionViewXform *  modelXform * vec4(vertex_position, 1);
}
