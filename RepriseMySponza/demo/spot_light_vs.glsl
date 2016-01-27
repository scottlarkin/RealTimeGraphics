#version 330

struct SpotLight
{
	vec3 position;
	float pad;
	mat4 projectionView;
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

out vec4 shadowCoord;
flat out int instID; 

out VS_OUT {
    vec3 FragPos;
    vec4 FragPosLightSpace;
} vs_out;

layout (location = 0) in vec3 vertex_position;

void main(void)
{

	instID = gl_InstanceID;
	SpotLight light = lights[instID];

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

	//shadowCoord = light.projectionView * mat * vec4(vertex_position, 1);

	vs_out.FragPos = vec3(mat * vec4(vertex_position, 1.0));
	vs_out.FragPosLightSpace = light.projectionView * vec4(vs_out.FragPos, 1.0);

    gl_Position = projectionViewXform * mat * vec4(vertex_position, 1);
}
