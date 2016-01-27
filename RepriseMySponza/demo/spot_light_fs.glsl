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

flat in vec3 lightIntensity;
flat in vec3 lightPosition;
in float lightRange;
flat in vec3 lightDirection;
in float lightFoV;

uniform sampler2DArray sampler_shadowMap;
uniform sampler2DRect sampler_world_position;
uniform sampler2DRect sampler_world_normal;
uniform sampler2DRect sampler_world_matColour;

layout (location = 0) out vec4 reflected_light;

in vec4 shadowCoord;
flat in int instID; 

in VS_OUT {
    vec3 FragPos;
    vec4 FragPosLightSpace;
} fs_in;


float ShadowCalculation(vec4 fragPosLightSpace, int i)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5; 
    float closestDepth = texture(sampler_shadowMap, vec3(projCoords.xy, i )).r;   
    float currentDepth = projCoords.z; 
    float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;  
    return shadow;
}

void main(void)
{

	vec3 position = texelFetch(sampler_world_position, ivec2(gl_FragCoord.xy)).xyz;
	vec2 normalXY = texelFetch(sampler_world_normal, ivec2(gl_FragCoord.xy)).xy;
	vec4 matColour = texelFetch(sampler_world_matColour, ivec2(gl_FragCoord.xy));

	vec3 diffuse = vec3(0,0,0); 
	
	vec3 normal = vec3(normalXY.x, normalXY.y, 1-(normalXY.x+normalXY.y));
	
	int matId = int(matColour.x);
	
	vec3 bulb_colour;
	
	vec3 L = normalize(lightPosition - position);
	vec3 R = normalize(reflect(-L, normal));
	
	float spotEffect = dot(normalize(L), normalize(-lightDirection));
	float distance = length(lightPosition - position);
	float LdotN = dot(normal,L);
	
	//attenuation
	float cone_attenuation = smoothstep(cos(0.9* lightFoV * 0.0174532925 ), 1 ,spotEffect);
	float distance_attenuation = smoothstep(lightRange, 1, distance );
	float attenuation = cone_attenuation * distance_attenuation;

	//diffuse
	if(spotEffect > cos(lightFoV * 0.0174532925)){

		bulb_colour = materials[matId].dColour * (lightIntensity) * attenuation;
		diffuse += (clamp(LdotN,0,1) * bulb_colour);
	}

	float shadow = ShadowCalculation(fs_in.FragPosLightSpace,instID );       

	reflected_light = vec4( diffuse * 1,0);
}
