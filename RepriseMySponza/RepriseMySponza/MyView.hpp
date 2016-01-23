#pragma once

#include <SceneModel/SceneModel_fwd.hpp>
#include <tygra/WindowViewDelegate.hpp>
#include <tgl/tgl.h>
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include <map>
#include <SceneModel/SceneModel.hpp>


class MyView : public tygra::WindowViewDelegate
{
public:

	MyView();

	~MyView();

	void
		setScene(std::shared_ptr<const SceneModel::Context> scene);

private:
	enum SHADER_TYPE{
		PRE_SHADER = 0,
		GLOBAL_LIGHT = 1,
		SPOT_LIGHT = 2,
		POINT_LIGHT = 3,
		POST_SHADER = 4,
		MAX_SHADERS = POST_SHADER + 1
	};

	enum VERTEX_ATTRIBUTE{
		POSITION = 0,
		NORMAL = 1,
		TEX_COORD = 2,
		MATERIAL_COLOUR = 3,
		X_FORM = 4
	};

	void
		windowViewWillStart(std::shared_ptr<tygra::Window> window) override;

	void
		windowViewDidReset(std::shared_ptr<tygra::Window> window,
		int width,
		int height) override;

	void
		windowViewDidStop(std::shared_ptr<tygra::Window> window) override;

	void
		windowViewRender(std::shared_ptr<tygra::Window> window) override;

	GLuint getVertexShader(std::string file);
	GLuint getFragmentShader(std::string file);
	void addShaderProgram(SHADER_TYPE type, std::string vsFile, std::string fsFile);

	std::shared_ptr<const SceneModel::Context> scene_;

	std::vector<GLuint> shaderPrograms_;

	struct vertex
	{
		glm::vec3 pos;
		glm::vec3 normal;
		glm::vec2 texCoords;
	};

	struct pointLightInstanceData
	{
		glm::vec3 position;
		float range;
		glm::mat4 xForm;
	};
	
	struct instanceData
	{
		float matColour;
		glm::mat4 xForm;
	};

	struct LightMesh
	{
		GLuint vertex_vbo;
		GLuint element_vbo;
		GLuint vao;
		int element_count;
		GLuint instance_data_vbo;

		LightMesh() :	vertex_vbo(0),
						element_vbo(0),
						vao(0),
						element_count(0) {}

	};

	LightMesh light_quad_mesh_; // vertex array of vec2 position
	LightMesh light_sphere_mesh_; // element array into vec3 position
	LightMesh light_cone_mesh_; // element array into vec3 position

	void generateLightMeshes();

	struct Mesh
	{
		GLuint element_vbo;
		GLuint vertex_data_vbo;
		GLuint instance_data_vbo;
		GLuint vao;
		int element_count;
		int id;
		std::vector<int> instanceIDs;
		Mesh() :

			element_vbo(0),
			vertex_data_vbo(0),
			vao(0),
			element_count(0) {}
	};

	std::vector<Mesh> meshes_;

	float aspect_ratio_;
	glm::vec3 upDir_;
	glm::mat4 projection_xform_;

	struct PerFrameUniforms
	{
		glm::mat4 projectionViewXform;
		glm::vec3 ambientColour;
		float pad;
		glm::vec3 cameraPos;
	};

	struct PerLightUniforms
	{
		glm::vec3 lightDirection;
		float lightFoV;
		glm::vec3 lightPosition;
		float lightRange;
		glm::vec3 lightIntensity;
	};

	struct Material{
		glm::vec3 dColour;
		float shininess;
		glm::vec3 sColour;
		float pad;
	};

	GLuint perLight_ubo_;
	//GLuint perModel_ubo_;
	GLuint perFrame_ubo_;
	GLuint material_ubo_;

	//deferred shading stuff
	GLuint gbuffer_position_tex_;
	GLuint gbuffer_normal_tex_;
	GLuint gbuffer_depth_tex_;
	GLuint gbuffer_matColour_tex_;

	GLuint lbuffer_fbo_;
	GLuint lbuffer_colour_rbo_;

	GLuint gbuffer_depth_rbo_;
	GLuint gbuffer_fbo_;

	int width_;
	int height_;

};
