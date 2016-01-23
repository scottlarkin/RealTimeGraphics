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

layout (std140) uniform PerFrameUniforms
{
	mat4 projectionViewXform;
	vec3 cameraPos;
};


in flat vec3 lightIntensity;
in flat vec3 lightPosition;
in flat float lightRange;

uniform sampler2DRect sampler_world_position;
uniform sampler2DRect sampler_world_normal;
uniform sampler2DRect sampler_world_matColour;

layout (location = 0) out vec4 reflected_light;

void main(void)
{

	vec3 position = texelFetch(sampler_world_position, ivec2(gl_FragCoord.xy)).xyz;
	vec2 normalXY = texelFetch(sampler_world_normal, ivec2(gl_FragCoord.xy)).xy;
	vec4 matColour = texelFetch(sampler_world_matColour, ivec2(gl_FragCoord.xy));

	vec3 diffuse; 
	
	vec3 normal = vec3(normalXY.x, normalXY.y, 1-(normalXY.x+normalXY.y));
	
	int matId = int(matColour.x);
	
	vec3 bulb_colour = materials[matId].dColour * lightIntensity;

	vec3 L = normalize(lightPosition - position);
	vec3 R = normalize(reflect(-L, normal));
	
	float distance = length(lightPosition - position);
	float LdotN = dot(normal,L);
	
	//attenuation
	float attenuation = smoothstep(lightRange, 1, distance );

	//diffuse
	bulb_colour *= attenuation * 1  ;
	diffuse += (clamp(LdotN,0,1) * bulb_colour);
	
	reflected_light = vec4(diffuse,0);
}
