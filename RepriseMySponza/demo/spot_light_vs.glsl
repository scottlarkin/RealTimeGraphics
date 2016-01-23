#version 330

struct SpotLight
{
	vec3 position;
	float pad;
	vec3 direction;
	float FoV;
	vec3 intensity;
	float range;
};

layout (std140) uniform SpotLightUniforms
{
	SpotLight lights[$spotLightCount];
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
out flat vec3 lightDirection;
out flat float lightFoV;

layout (location = 0) in vec3 vertex_position;

void main(void)
{
	SpotLight light = lights[gl_InstanceID];

	lightIntensity = light.intensity;
	lightPosition = light.position;
	lightRange = light.range;
	lightDirection = light.direction;
	lightFoV = light.FoV;

	mat4 mat;

	mat[0] = vec4(lightRange,0,0,0);
	mat[1] = vec4(0,lightRange,0,0);
	mat[2] = vec4(0,0,lightRange,0);
	mat[3] = vec4(lightPosition, 1);

    gl_Position = projectionViewXform * mat * vec4(vertex_position, 1);
}
