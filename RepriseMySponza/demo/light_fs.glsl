#version 330

layout (std140) uniform PerFrameUniforms
{
	mat4 projectionViewXform;
	vec3 ambientColour;
	float pad;
	vec3 cameraPos;
};

layout (std140) uniform PerLightUniforms
{
	vec3 lightDirection;
	float lightFoV;
	vec3 lightPosition;
	float lightRange;
	vec3 lightIntensity;
};

out vec4 fragment_colour;

in vec3 varying_position;
in vec3 varying_normal;
in vec2 varying_texCoords;
in vec3 varying_mat_colour;
in vec3 varying_spec_colour;
in float varying_mat_shininess;

void main(void)
{

	vec3 tex_colour = vec3(1,1,1);

	vec3 diffuse= vec3(0,0,0);
	vec3 specular= vec3(0,0,0);

	vec3 bulb_colour;

	vec3 L = normalize(lightPosition - varying_position);
	
	float spotEffect = dot(normalize(L), normalize(-lightDirection));
	float distance = length(lightPosition - varying_position);
	float LdotN = dot(varying_normal,L);
	
	//attenuation
	float cone_attenuation = smoothstep(cos(0.6* lightFoV * 0.0174532925 ), 1 ,spotEffect);
	float distance_attenuation = smoothstep(lightRange, 1, distance );
	float attenuation = cone_attenuation * distance_attenuation;
	
	//diffuse
	if(spotEffect > cos(lightFoV * 0.0174532925)){
		bulb_colour = varying_mat_colour * attenuation * lightIntensity  ;
		diffuse += (clamp(LdotN,0,1) * bulb_colour);
	}

	fragment_colour = vec4( diffuse ,0.0);
	
	//fragment_colour = vec4( varying_mat_colour,0.0);

}
