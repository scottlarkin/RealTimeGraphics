#version 330

struct DirectionalLight
{
	vec3 direction;
	float pada;
	vec3 intensity;
	float padb;
};

layout (std140) uniform DirectionalLightUniforms
{
	DirectionalLight lights[$directionalLightCount];
};

layout (std140) uniform PerFrameUniforms
{
	mat4 projectionViewXform;
	vec3 cameraPos;
};

layout (location = 0) in vec2 vertex_position;

flat out vec3 lightDirection;
flat out vec3 lightIntensity;

void main(void)
{

	DirectionalLight light = lights[gl_InstanceID];

	lightDirection = light.direction;
	lightIntensity = light.intensity;

    gl_Position = vec4(vertex_position, 0.0, 1.0);
}
