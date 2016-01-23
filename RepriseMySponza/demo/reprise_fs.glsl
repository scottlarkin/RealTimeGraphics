#version 330

struct spotLight
{
    vec3 position;
    vec3 direction;
    float field_of_view;
    float range;
	vec3 intensity;
};

struct pointLight
{
    vec3 position;
    float range;
	vec3 intensity;
};

layout (std140) uniform PerFrameUniforms
{
	mat4 projectionViewXform;
	vec3 ambientColour;
	float pad;
	vec3 cameraPos;
};

layout (std140) uniform PerModelUniforms
{
	mat4 modelXform;
	vec3 materialColour;
	float materialShininess;
	vec3 materialSpecularColour;
};

uniform spotLight light_source[5];
uniform pointLight point_light_source[20];

out vec4 fragment_colour;

in vec3 varying_position;
in vec3 varying_normal;
in vec2 varying_texCoords;
in vec3 varying_mat_colour;

void main(void)
{

  vec3 tex_colour = vec3(1,1,1);

	vec3 ambient = vec3(0,0,0);
	vec3 diffuse= vec3(0,0,0);
	vec3 specular= vec3(0,0,0);

	ambient = (ambientColour);

	vec3 V  = normalize(cameraPos - varying_position);

	for(int i = 0; i < 20; i++)//for each point light
	{
		vec3 bulb_colour;

		vec3 L = normalize(point_light_source[i].position - varying_position);
		vec3 R = normalize(reflect(-L, varying_normal));
		 
		float distance = length(point_light_source[i].position - varying_position);
		float LdotN = dot(varying_normal,L);

		float attenuation = smoothstep(point_light_source[i].range, 1, distance );
		//diffuse
		bulb_colour = varying_mat_colour * attenuation * point_light_source[i].intensity  ;
		diffuse += (clamp(LdotN,0,1) * bulb_colour);

		//specular
		if(materialShininess != 0 && LdotN > 0){
			specular +=  materialSpecularColour * tex_colour * pow(max((dot(R,V)),0.0), materialShininess) * attenuation;
		}	

	}
		
	for(int i = 0; i < 5; i++)//for each spot light
	{
		vec3 bulb_colour;

		vec3 L = normalize(light_source[i].position - varying_position);
		vec3 R = normalize(reflect(-L, varying_normal));
		
		float spotEffect = dot(normalize(L), normalize(-light_source[i].direction));
 
		float distance = length(light_source[i].position - varying_position);

		float LdotN = dot(varying_normal,L);

		//attenuation
		float cone_attenuation = smoothstep(cos(0.6* light_source[i].field_of_view * 0.0174532925 ), 1 ,spotEffect);
		float distance_attenuation = smoothstep(light_source[i].range, 1, distance );
		float attenuation = cone_attenuation * distance_attenuation;

		//diffuse
		if(spotEffect > cos(light_source[i].field_of_view * 0.0174532925)){

			bulb_colour = varying_mat_colour * attenuation * light_source[i].intensity  ;

			diffuse += (clamp(LdotN,0,1) * bulb_colour);

		}

		//specular
		if(materialShininess != 0 && LdotN > 0){
			specular +=  materialSpecularColour * tex_colour * pow(max((dot(R,V)),0.0), materialShininess) * attenuation;
		}	

	}
	
	fragment_colour = vec4( diffuse + ambient + specular,0.0);
	//fragment_colour = vec4( varying_normal,0.0);
}
