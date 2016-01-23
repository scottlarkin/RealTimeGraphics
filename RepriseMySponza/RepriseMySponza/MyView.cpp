#include "MyView.hpp"
#include <SceneModel/SceneModel.hpp>
#include <tygra/FileHelper.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <cassert>
#include <tsl/primitives.hpp>

MyView::
MyView()
{
}

MyView::
~MyView() {
}

void MyView::
setScene(std::shared_ptr<const SceneModel::Context> scene)
{
	scene_ = scene;
}


void MyView::generateLightMeshes()
{
	//full screen quad (directional / ambient lighting)
	{
		std::vector<glm::vec2> vertices(4);
		vertices[0] = glm::vec2(-1, -1);
		vertices[1] = glm::vec2(1, -1);
		vertices[2] = glm::vec2(1, 1);
		vertices[3] = glm::vec2(-1, 1);

		glGenBuffers(1, &light_quad_mesh_.vertex_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, light_quad_mesh_.vertex_vbo);
		glBufferData(GL_ARRAY_BUFFER,
			vertices.size() * sizeof(glm::vec2),
			vertices.data(),
			GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glGenVertexArrays(1, &light_quad_mesh_.vao);
		glBindVertexArray(light_quad_mesh_.vao);
		glBindBuffer(GL_ARRAY_BUFFER, light_quad_mesh_.vertex_vbo);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,
			sizeof(glm::vec2), 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	//sphere (point light)
	{
		tsl::IndexedMesh mesh;
		tsl::CreateSphere(1.f, 12, &mesh);
		tsl::ConvertPolygonsToTriangles(&mesh);

		light_sphere_mesh_.element_count = mesh.index_array.size();

		glGenBuffers(1, &light_sphere_mesh_.vertex_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, light_sphere_mesh_.vertex_vbo);
		glBufferData(GL_ARRAY_BUFFER,
			mesh.vertex_array.size() * sizeof(glm::vec3),
			mesh.vertex_array.data(),
			GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glGenBuffers(1, &light_sphere_mesh_.element_vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, light_sphere_mesh_.element_vbo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,
			mesh.index_array.size() * sizeof(unsigned int),
			mesh.index_array.data(),
			GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		glGenVertexArrays(1, &light_sphere_mesh_.vao);
		glBindVertexArray(light_sphere_mesh_.vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, light_sphere_mesh_.element_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, light_sphere_mesh_.vertex_vbo);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	//cone (spot light)...ideally, however im using a sphere instead and checking in the FS if fragment is within the cone
	{
		tsl::IndexedMesh mesh;
		tsl::CreateCone(1.f, 1.f, 12, &mesh);
		tsl::ConvertPolygonsToTriangles(&mesh);

		light_cone_mesh_.element_count = mesh.index_array.size();

		glGenBuffers(1, &light_cone_mesh_.vertex_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, light_cone_mesh_.vertex_vbo);
		glBufferData(GL_ARRAY_BUFFER,
			mesh.vertex_array.size() * sizeof(glm::vec3),
			mesh.vertex_array.data(),
			GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glGenBuffers(1, &light_cone_mesh_.element_vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, light_cone_mesh_.element_vbo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,
			mesh.index_array.size() * sizeof(unsigned int),
			mesh.index_array.data(),
			GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		glGenVertexArrays(1, &light_cone_mesh_.vao);
		glBindVertexArray(light_cone_mesh_.vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, light_cone_mesh_.element_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, light_cone_mesh_.vertex_vbo);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
			sizeof(glm::vec3), 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

}

GLuint MyView::getVertexShader(std::string file){

	GLint compile_status = 0;
	GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	std::string vertex_shader_string = tygra::stringFromFile(file);
	const char *vertex_shader_code = vertex_shader_string.c_str();
	glShaderSource(vertex_shader, 1, (const GLchar **)&vertex_shader_code, NULL);
	glCompileShader(vertex_shader);
	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &compile_status);

	if (compile_status != GL_TRUE) {
		const int string_length = 1024;
		GLchar log[string_length] = "";
		glGetShaderInfoLog(vertex_shader, string_length, NULL, log);
		std::cerr << log << std::endl;
	}

	return vertex_shader;

}

GLuint MyView::getFragmentShader(std::string file){

	GLint compile_status = 0;
	GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	std::string fragment_shader_string = tygra::stringFromFile(file);
	const char *fragment_shader_code = fragment_shader_string.c_str();
	glShaderSource(fragment_shader, 1, (const GLchar **)&fragment_shader_code, NULL);
	glCompileShader(fragment_shader);
	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &compile_status);

	if (compile_status != GL_TRUE) {
		const int string_length = 1024;
		GLchar log[string_length] = "";
		glGetShaderInfoLog(fragment_shader, string_length, NULL, log);
		std::cerr << log << std::endl;
	}

	return fragment_shader;

}

void MyView::addShaderProgram(SHADER_TYPE type, std::string vsFile, std::string fsFile){
	GLuint program, fs, vs;

	program = glCreateProgram();
	fs = getFragmentShader(fsFile);
	vs = getVertexShader(vsFile);

	glAttachShader(program, vs);
	glDeleteShader(vs);
	glAttachShader(program, fs);
	//for (int i = 0; i < fragmentOutputs.size(); i++){
	//	glBindFragDataLocation(program, i, fragmentOutputs[i].c_str());
	//}
	glDeleteShader(fs);
	glLinkProgram(program);

	//test program
	GLint link_status = 0;
	glGetProgramiv(program, GL_LINK_STATUS, &link_status);
	if (link_status != GL_TRUE) {
		const int string_length = 1024;
		GLchar log[string_length] = "";
		glGetProgramInfoLog(program, string_length, NULL, log);
		std::cerr << log << std::endl;
	}

	shaderPrograms_[type] = program;
}

void MyView::
windowViewWillStart(std::shared_ptr<tygra::Window> window)
{
	assert(scene_ != nullptr);
	shaderPrograms_.resize(MAX_SHADERS);

	addShaderProgram(PRE_SHADER, "pre_vs.glsl", "pre_fs.glsl");
	addShaderProgram(GLOBAL_LIGHT, "global_light_vs.glsl", "global_light_fs.glsl");
	addShaderProgram(POINT_LIGHT, "point_light_vs.glsl", "point_light_fs.glsl");

	std::vector<SceneModel::Mesh> meshes = SceneModel::GeometryBuilder().getAllMeshes();
	std::vector<SceneModel::Instance> instances = scene_->getAllInstances();

	spotLightCount_ = scene_->getAllSpotLights().size();
	pointLightCount_ = scene_->getAllPointLights().size();
	directionalLightCount_ = scene_->getAllDirectionalLights().size();

	Mesh mesh;
	//for each mesh
	for (int i = 0; i < meshes.size(); i++)
	{
		std::vector<glm::vec3> positions = meshes[i].getPositionArray();
		std::vector<glm::vec3> normals = meshes[i].getNormalArray();
		std::vector<glm::vec2> texCoords = meshes[i].getTextureCoordinateArray();
		std::vector<unsigned int> elements = meshes[i].getElementArray();

		std::vector<vertex> vertexData;

		bool hasTexCoords = texCoords.size() > 0;

		for (int j = 0; j < positions.size(); j++)
		{
			vertex v;
			v.pos = positions[j];
			v.normal = normals[j];

			if (hasTexCoords)
			{
				v.texCoords = texCoords[j];
			}
			else
			{
				//mesh has no tex coords... fill with 0s for now, refactor later
				v.texCoords = glm::vec2(0, 0);
			}

			vertexData.push_back(v);
		}

		glGenBuffers(1, &mesh.vertex_data_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, mesh.vertex_data_vbo);
		glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(vertex), vertexData.data(), GL_STATIC_DRAW);

		glGenBuffers(1, &mesh.element_vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.element_vbo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, elements.size() * sizeof(unsigned int), elements.data(), GL_STATIC_DRAW);
		mesh.element_count = elements.size();

		glGenVertexArrays(1, &mesh.vao);
		glBindVertexArray(mesh.vao);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.element_vbo);

		glEnableVertexAttribArray(POSITION);
		glVertexAttribPointer(POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), TGL_BUFFER_OFFSET_OF(vertex, pos));

		glEnableVertexAttribArray(NORMAL);
		glVertexAttribPointer(NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), TGL_BUFFER_OFFSET_OF(vertex, normal));

		glEnableVertexAttribArray(TEX_COORD);
		glVertexAttribPointer(TEX_COORD, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), TGL_BUFFER_OFFSET_OF(vertex, texCoords));

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		std::vector<InstanceData> instancesData;

		mesh.instanceIDs.clear();

		for (int x = 0; x < instances.size(); x++){
			if (instances[x].getMeshId() == meshes[i].getId()){

				InstanceData id;
				id.xForm = glm::mat4(instances[x].getTransformationMatrix());
				id.matColour = (float)instances[x].getMaterialId() - 200;
				instancesData.push_back(id);
				mesh.instanceIDs.push_back(instances[x].getId());
			}
		}

		int numInstances = instancesData.size();
		GLsizei vec4Size = sizeof(glm::vec4);

		//instanced data
		glGenBuffers(1, &mesh.instance_data_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, mesh.instance_data_vbo);
		glBufferData(GL_ARRAY_BUFFER, numInstances * sizeof(InstanceData), instancesData.data(), GL_STREAM_DRAW);

		glEnableVertexAttribArray(MATERIAL_COLOUR);
		glVertexAttribPointer(MATERIAL_COLOUR, 1, GL_FLOAT, GL_FALSE, sizeof(InstanceData), TGL_BUFFER_OFFSET_OF(InstanceData, matColour));
		glVertexAttribDivisor(MATERIAL_COLOUR, 1);

		//mat4 needs to be passed as 4 lots of vec4s
		for (int n = 0; n < 4; n++){
			glEnableVertexAttribArray(X_FORM + n);
			glVertexAttribPointer(X_FORM + n, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData), TGL_BUFFER_OFFSET_OF(InstanceData, xForm) + (n * vec4Size));
			glVertexAttribDivisor(X_FORM + n, 1);
		}

		glBindVertexArray(0);

		meshes_.push_back(mesh);
	}

	glEnable(GL_CULL_FACE);
	glClearColor(0.f, 0.f, 0.25f, 0.f);

	std::vector<SceneModel::Material> mats = scene_->getAllMaterials();
	
	//load the materials ubo data
	std::vector<Material> materials;
	for (int i = 0; i < mats.size(); i++){
		
		Material m;
		m.dColour = mats[i].getDiffuseColour();
		m.sColour = mats[i].getSpecularColour();
		m.shininess = mats[i].getShininess();

		materials.push_back(m);
	}

	//load the point light ubo data
	std::vector<SceneModel::PointLight> pls = scene_->getAllPointLights();
	std::vector<PointLight> pointLights;
	for (int i = 0; i < pointLightCount_; i++){

		glm::mat4 xform = glm::mat4();
		float s = pls[i].getRange();
		glm::vec3 pos = pls[i].getPosition();

		//scale and transform light souce to the correct position and size
		xform = glm::translate(xform, pos);
		xform = glm::scale(xform, glm::vec3(s, s, s));

		PointLight pl;
		pl.intensity = pls[i].getIntensity();
		pl.xForm = xform;

		pointLights.push_back(pl);
	}

	//unniform buffer stuff
	glGenBuffers(1, &perFrame_ubo_);
	glBindBuffer(GL_UNIFORM_BUFFER, perFrame_ubo_);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(PerFrameUniforms), nullptr, GL_DYNAMIC_DRAW);

	glGenBuffers(1, &material_ubo_);
	glBindBuffer(GL_UNIFORM_BUFFER, material_ubo_);
	glBufferData(GL_UNIFORM_BUFFER, materials.size() * sizeof(Material), materials.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &pointLight_ubo_);
	glBindBuffer(GL_UNIFORM_BUFFER, pointLight_ubo_);
	glBufferData(GL_UNIFORM_BUFFER, pointLightCount_ * sizeof(PointLight), pointLights.data(), GL_STREAM_DRAW);

	glBindBufferBase(GL_UNIFORM_BUFFER, 0, perFrame_ubo_);
	glUniformBlockBinding(shaderPrograms_[PRE_SHADER], glGetUniformBlockIndex(shaderPrograms_[PRE_SHADER], "PerFrameUniforms"), 0);

	glBindBufferBase(GL_UNIFORM_BUFFER, 1, material_ubo_);
	glUniformBlockBinding(shaderPrograms_[GLOBAL_LIGHT], glGetUniformBlockIndex(shaderPrograms_[GLOBAL_LIGHT], "MaterialUniforms"), 1);
	glUniformBlockBinding(shaderPrograms_[POINT_LIGHT], glGetUniformBlockIndex(shaderPrograms_[POINT_LIGHT], "MaterialUniforms"), 1);

	glBindBufferBase(GL_UNIFORM_BUFFER, 2, pointLight_ubo_);
	glUniformBlockBinding(shaderPrograms_[POINT_LIGHT], glGetUniformBlockIndex(shaderPrograms_[POINT_LIGHT], "PointLightUniforms"), 2);

	//http://stackoverflow.com/questions/9155044/opengl-uniform-buffers

	//GBUFER TEXTURES
	glGenTextures(1, &gbuffer_position_tex_);
	glGenTextures(1, &gbuffer_normal_tex_);
	glGenTextures(1, &gbuffer_matColour_tex_);

	glGenFramebuffers(1, &lbuffer_fbo_);
	glGenRenderbuffers(1, &lbuffer_colour_rbo_);

	glGenFramebuffers(1, &gbuffer_fbo_);
	glGenRenderbuffers(1, &gbuffer_depth_rbo_);

	generateLightMeshes();
}

void MyView::
windowViewDidReset(std::shared_ptr<tygra::Window> window,
int width,
int height)
{
	glViewport(0, 0, width, height);

	width_ = width;
	height_ = height;

	upDir_ = scene_->getUpDirection();

	float far = scene_->getCamera().getFarPlaneDistance();
	float near = scene_->getCamera().getNearPlaneDistance();
	float fov = scene_->getCamera().getVerticalFieldOfViewInDegrees();

	GLint viewport_size[4];
	glGetIntegerv(GL_VIEWPORT, viewport_size);
	aspect_ratio_ = viewport_size[2] / (float)viewport_size[3];
	projection_xform_ = glm::perspective(fov, aspect_ratio_, near, far);


	//deffered stuff
	//g-buffer

	glBindFramebuffer(GL_FRAMEBUFFER, gbuffer_fbo_);
	//position
	//store full-fat position 
	glBindTexture(GL_TEXTURE_RECTANGLE, gbuffer_position_tex_);
	glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE, gbuffer_position_tex_, 0);

	//normal. Only store x and y, reconstruct z in the fragment shader
	glBindTexture(GL_TEXTURE_RECTANGLE, gbuffer_normal_tex_);
	glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RG32F, width, height, 0, GL_RG, GL_FLOAT, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_RECTANGLE, gbuffer_normal_tex_, 0);

	//mat colour. Store an index to an array of materials stored in a ubo
	glBindTexture(GL_TEXTURE_RECTANGLE, gbuffer_matColour_tex_);
	glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_R16F, width, height, 0, GL_RED, GL_FLOAT, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_RECTANGLE, gbuffer_matColour_tex_, 0);

	//total storage per texel = 96 + 64 + 16 bits = 22 bytes
	//full HD G-buffer sohuld use 45.6MB of GPU memory

	glBindRenderbuffer(GL_RENDERBUFFER, gbuffer_depth_rbo_);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, gbuffer_depth_rbo_);

	GLuint framebuffer_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (framebuffer_status != GL_FRAMEBUFFER_COMPLETE) {
		tglDebugMessage(GL_DEBUG_SEVERITY_HIGH_ARB, "framebuffer not complete");
	}

	GLenum buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, buffers);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	//l-buffer
	glBindFramebuffer(GL_FRAMEBUFFER, lbuffer_fbo_);
	glBindRenderbuffer(GL_RENDERBUFFER, lbuffer_colour_rbo_);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_RGB32F, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, lbuffer_colour_rbo_);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, gbuffer_depth_rbo_);

	framebuffer_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (framebuffer_status != GL_FRAMEBUFFER_COMPLETE) {
		tglDebugMessage(GL_DEBUG_SEVERITY_HIGH_ARB, "framebuffer not complete");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void MyView::
windowViewDidStop(std::shared_ptr<tygra::Window> window)
{
}

void MyView::
windowViewRender(std::shared_ptr<tygra::Window> window)
{
	assert(scene_ != nullptr);

	GLuint program;

	//update the xform of instances each frame because some move around a bit
	for (int i = 0; i < meshes_.size(); i++)
	{
		glBindBuffer(GL_ARRAY_BUFFER, meshes_[i].instance_data_vbo);

		for (int j = 0; j < meshes_[i].instanceIDs.size(); j++){
			glm::mat4 m = glm::mat4(scene_->getInstanceById(meshes_[i].instanceIDs[j]).getTransformationMatrix());

			int offset = sizeof(float) + (j * sizeof(InstanceData));

			glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(glm::mat4), &m);
		}
	}

	SceneModel::Camera cam = scene_->getCamera();
	glm::vec3 camPos = cam.getPosition();
	glm::vec3 CamLookAt = camPos + cam.getDirection();
	glm::mat4 view_xform = glm::lookAt(camPos, CamLookAt, upDir_);

	PerFrameUniforms perFrameUniforms;
	perFrameUniforms.projectionViewXform = projection_xform_ * view_xform;
	perFrameUniforms.ambientColour = scene_->getAmbientLightIntensity();
	perFrameUniforms.cameraPos = camPos;

	glBindBuffer(GL_UNIFORM_BUFFER, perFrame_ubo_);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(PerFrameUniforms), &perFrameUniforms);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, gbuffer_fbo_);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LESS);

	glDisable(GL_BLEND);

	std::vector<SceneModel::PointLight> pointLights = scene_->getAllPointLights();
	std::vector<SceneModel::SpotLight> spotLights = scene_->getAllSpotLights();

	//G-buffer geometry pass

	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_ALWAYS, 0, ~0);
	glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);

	program = shaderPrograms_[PRE_SHADER];
	glUseProgram(program);

	Mesh mesh;

	for (int i = 0; i < meshes_.size(); i++)
	{
		mesh = meshes_[i];
		glBindVertexArray(mesh.vao);
		glDrawElementsInstanced(GL_TRIANGLES, mesh.element_count, GL_UNSIGNED_INT, 0, mesh.instanceIDs.size());
	}

	glStencilFunc(GL_NOTEQUAL, 0, ~0);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

	//	//lighting passes
	glBindFramebuffer(GL_FRAMEBUFFER, lbuffer_fbo_);
	glClear(GL_COLOR_BUFFER_BIT);

	glDisable(GL_DEPTH_TEST);

	//	//global lights
	program = shaderPrograms_[GLOBAL_LIGHT];
	glUseProgram(program);
	std::vector<SceneModel::DirectionalLight> dLights = scene_->getAllDirectionalLights();

	GLint posTex = glGetUniformLocation(program, "sampler_world_position");
	glUniform1i(posTex, 0);

	GLint normTex = glGetUniformLocation(program, "sampler_world_normal");
	glUniform1i(normTex, 1);

	GLint matColTex = glGetUniformLocation(program, "sampler_world_matColour");
	glUniform1i(matColTex, 2);

	//bind textures to tex units 0 and 1
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_RECTANGLE, gbuffer_position_tex_);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_RECTANGLE, gbuffer_normal_tex_);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_RECTANGLE, gbuffer_matColour_tex_);

	glBindVertexArray(light_quad_mesh_.vao);
	for (int i = 0; i < dLights.size(); i++){

		glm::vec3 dir = dLights[i].getDirection();

		GLuint lightDir = glGetUniformLocation(program, "light_direction");
		glUniform3fv(lightDir, 1, glm::value_ptr(dir));

		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	}

	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_ONE, GL_ONE);

	//point lights
	program = shaderPrograms_[POINT_LIGHT];
	glUseProgram(program);

	posTex = glGetUniformLocation(program, "sampler_world_position");
	glUniform1i(posTex, 0);

	normTex = glGetUniformLocation(program, "sampler_world_normal");
	glUniform1i(normTex, 1);

	matColTex = glGetUniformLocation(program, "sampler_world_matColour");
	glUniform1i(matColTex, 2);

	//Draw lights instanced
	glBindVertexArray(light_sphere_mesh_.vao);
	glDrawElementsInstanced(GL_TRIANGLES, light_sphere_mesh_.element_count, GL_UNSIGNED_INT, 0, pointLightCount_);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, lbuffer_fbo_);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBlitFramebuffer(0, 0, width_, height_, 0, 0, width_, height_, GL_COLOR_BUFFER_BIT, GL_LINEAR);
}
