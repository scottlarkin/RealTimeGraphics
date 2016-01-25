
#define STRINGIFY(s) #s

std::string header_vs = 
"#version 410 compatibility\n\
 #ifndef SMAA_PIXEL_SIZE\n\
 #define SMAA_PIXEL_SIZE vec2(1.0 / 1280.0, 1.0 / 720.0)\n\
 #endif\n\
 #define SMAA_PRESET_ULTRA 1\n\
 #define SMAA_GLSL_4 1\n\
 #define SMAA_ONLY_COMPILE_VS 1\n\
 #include \"SMAA.h\"\n\
";

std::string header_ps = 
"#version 410 compatibility\n\
 #ifndef SMAA_PIXEL_SIZE\n\
 #define SMAA_PIXEL_SIZE vec2(1.0 / 1280.0, 1.0 / 720.0)\n\
 #endif\n\
 #define SMAA_PRESET_ULTRA 1\n\
 #define SMAA_GLSL_4 1\n\
 #define SMAA_ONLY_COMPILE_PS 1\n\
 #include \"SMAA.h\"\n\
";

std::string edge_vs = 
header_vs +
STRINGIFY(
out vec2 texcoord; \n
out vec4 offset[3]; \n
out vec4 dummy2; \n

layout (location = 0) in vec2 vertex_position; \n

void main() \n
{ \n
  texcoord = (vertex_position * 0.5) + 0.5; \n
  vec4 dummy1 = vec4(0); \n
  SMAAEdgeDetectionVS(dummy1, dummy2, texcoord, offset); \n
  gl_Position = vec4(vertex_position.xy, 0.0, 1.0); \n
} \n
);

std::string edge_ps = 
header_ps +
STRINGIFY(
uniform sampler2D albedo_tex; \n
in vec2 texcoord; \n
in vec4 offset[3]; \n
in vec4 dummy2; \n

layout(location = 0) out vec4 fragCol; \n

void main() \n
{ \n
  hash if SMAA_PREDICATION == 1 \n
	fragCol = SMAAColorEdgeDetectionPS(texcoord, offset, albedo_tex, depthTex); \n
  hash else \n
	fragCol = SMAAColorEdgeDetectionPS(texcoord, offset, albedo_tex); \n
  hash endif \n
} \n
);

std::string blend_vs =
header_vs +
STRINGIFY(
out vec2 texcoord; \n
out vec2 pixcoord; \n
out vec4 offset[3]; \n
out vec4 dummy2; \n

layout(location = 0) in vec2 vertex_position; \n

void main() \n
{ \n
texcoord = (vertex_position * 0.5) + 0.5; \n
vec4 dummy1 = vec4(0); \n
SMAABlendingWeightCalculationVS(dummy1, dummy2, texcoord, pixcoord, offset); \n
gl_Position = vec4(vertex_position, 0.0, 1.0);; \n
} \n
);

std::string blend_ps = 
header_ps +
STRINGIFY(
uniform sampler2D edge_tex; \n
uniform sampler2D area_tex; \n
uniform sampler2D search_tex; \n
in vec2 texcoord; \n
in vec2 pixcoord; \n
in vec4 offset[3]; \n
in vec4 dummy2; \n

layout(location = 0) out vec4 fragCol; \n

void main() \n
{ \n
	fragCol = SMAABlendingWeightCalculationPS(texcoord, pixcoord, offset, edge_tex, area_tex, search_tex, ivec4(0)); \n
} \n
);

std::string neighborhood_vs = 
header_vs +
STRINGIFY(
out vec2 texcoord; \n
out vec4 offset[2]; \n
out vec4 dummy2; \n

layout(location = 0) in vec2 vertex_position; \n

void main() \n
{ \n
  texcoord = (vertex_position * 0.5) + 0.5; \n
  vec4 dummy1 = vec4(0); \n
  SMAANeighborhoodBlendingVS(dummy1, dummy2, texcoord, offset); \n
  gl_Position = vec4(vertex_position, 0.0, 1.0); \n
} \n
);

std::string neighborhood_ps = 
header_ps +
STRINGIFY(
uniform sampler2D albedo_tex; \n
uniform sampler2D blend_tex; \n
in vec2 texcoord; \n
in vec4 offset[2]; \n
in vec4 dummy2; \n

layout(location = 0) out vec4 fragCol; \n

void main() \n
{ \n
fragCol = SMAANeighborhoodBlendingPS(texcoord, offset, albedo_tex, blend_tex); \n
} \n
);
