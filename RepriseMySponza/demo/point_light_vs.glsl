#version 330

struct PointLight
{
	vec3 intensity;
	float pad;
	mat4 xform;
};

layout (std140) uniform PointLightUniforms
{
	PointLight lights[$pointLightCount];
};

layout (std140) uniform PerFrameUniforms
{
	mat4 projectionViewXform;
	vec3 ambientColour;
	float pad;
	vec3 cameraPos;
};

out flat vec3 lightPosition;
out flat float lightRange;
out flat vec3 lightIntensity;

layout (location = 0) in vec3 vertex_position;

void main(void)
{
	PointLight light = lights[gl_InstanceID];
	lightIntensity = light.intensity;
	lightPosition = light.xform[3].xyz;
	lightRange = length(light.xform[0].xyz);

    gl_Position = projectionViewXform * light.xform * vec4(vertex_position, 1);
}
