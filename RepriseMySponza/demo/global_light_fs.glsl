#version 330

struct Material{
	vec3 dColour;
	float shininess;
	vec3 sColour;
	float pad;
};

layout (std140) uniform MaterialUniforms
{
	Material materials[7];
};

layout (std140) uniform PerFrameUniforms
{
	mat4 projectionViewXform;
	vec3 ambientColour;
	float pad;
	vec3 cameraPos;
};

uniform sampler2DRect sampler_world_position;
uniform sampler2DRect sampler_world_normal;
uniform sampler2DRect sampler_world_matColour;

uniform vec3 light_direction = vec3(-3, -2, 1);
uniform float light_intensity = 0.15;
uniform vec3 ambientLight;

layout (location = 0) out vec4 reflected_light;

void main(void)
{
	vec4 position = texelFetch(sampler_world_position, ivec2(gl_FragCoord.xy));
	vec2 normalXY = texelFetch(sampler_world_normal, ivec2(gl_FragCoord.xy)).xy;
	vec4 matColour = texelFetch(sampler_world_matColour, ivec2(gl_FragCoord.xy));
	
	vec3 normal = vec3(normalXY.x, normalXY.y, 1-(normalXY.x + normalXY.y));
	
	int matId = int(matColour.x);
	
	vec3 diffuse = vec3(1,1,1) * materials[matId].dColour;

	float NdotL = max(dot(normalize(normal.xyz),light_direction),0.0);
	
	diffuse = diffuse * NdotL;

    reflected_light = vec4(diffuse,1);
}
