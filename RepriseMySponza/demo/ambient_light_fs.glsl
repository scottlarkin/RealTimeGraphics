#version 330

struct Material{
	vec3 dColour;
	float shininess;
	vec3 sColour;
	float pad;
};

layout (std140) uniform MaterialUniforms
{
	Material materials[$materialCount];
};

uniform sampler2DRect sampler_world_position;
uniform sampler2DRect sampler_world_normal;
uniform sampler2DRect sampler_world_matColour;

uniform vec3 ambientLight;

layout (location = 0) out vec3 reflected_light;

void main(void)
{
	vec4 position = texelFetch(sampler_world_position, ivec2(gl_FragCoord.xy));
	vec2 normalXY = texelFetch(sampler_world_normal, ivec2(gl_FragCoord.xy)).xy;
	vec4 matColour = texelFetch(sampler_world_matColour, ivec2(gl_FragCoord.xy));
	
	vec3 normal = vec3(normalXY.x, normalXY.y, 1-(normalXY.x + normalXY.y));
	
	int matId = int(matColour.x);
	
	vec3 diffuse = materials[matId].dColour * ambientLight;

	//float NdotL = max(dot(normalize(normal.xyz), lightDirection),0.0);
	
	//diffuse = diffuse * NdotL;

    reflected_light = diffuse;
}
