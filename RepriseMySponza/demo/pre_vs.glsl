#version 330

layout (std140) uniform PerFrameUniforms
{
	mat4 projectionViewXform;
	vec3 cameraPos;
};

layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_normal;
layout (location = 3) in float mat_colour;
layout (location = 4) in mat4 model_Xform;

out vec3 varying_position;
out vec3 varying_normal;
out vec2 varying_texCoords;
out flat float varying_mat_colour;


void main(void)
{
	varying_mat_colour = mat_colour;

    varying_position = vec3(model_Xform * vec4(vertex_position,1) ) ;
    varying_normal = vec3(model_Xform * vec4(vertex_normal,0) ) ;

    gl_Position = projectionViewXform * model_Xform * vec4(vertex_position, 1);
}
