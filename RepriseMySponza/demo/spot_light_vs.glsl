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
	vec3 cameraPos;
};

flat out vec3 lightPosition;
out float lightRange;
flat out vec3 lightIntensity;
flat out vec3 lightDirection;
out float lightFoV;

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

	mat[0] = vec4(5,0,0,0);
	mat[1] = vec4(0,5,0,0);
	mat[2] = vec4(0,0,5,0);
	mat[3] = vec4(lightPosition, 1);

    gl_Position = projectionViewXform * mat * vec4(vertex_position, 1);
}
