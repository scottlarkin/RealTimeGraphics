#pragma once

#include <SceneModel/SceneModel_fwd.hpp>
#include <tygra/WindowViewDelegate.hpp>
#include <tgl/tgl.h>
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include <map>
#include <SceneModel/SceneModel.hpp>

#include <iostream>
#include <fstream>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#undef near
#undef far
#endif

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

#define AREATEX_WIDTH 160
#define AREATEX_HEIGHT 560
#define SEARCHTEX_WIDTH 66
#define SEARCHTEX_HEIGHT 33

class MyView : public tygra::WindowViewDelegate
{
public:


	GLuint shadowMap_fbo_;
	GLuint shadowMap_rbo_;
	GLuint shadowMap_tex_;
	GLuint shadowMap_program_;


	bool StringReplace(std::string& str, const std::string& from, const std::string& to) {
		size_t start_pos = str.find(from);
		if (start_pos == std::string::npos)
			return false;
		str.replace(start_pos, from.length(), to);
		return true;
	}

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
		AMBIENT_LIGHT = 4,
		POST_SHADER = 5,
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

	struct PointLight
	{
		glm::vec3 intensity;
		float pad;
		glm::mat4 xForm;
	};

	struct DirectionalLight
	{
		glm::vec3 direction;
		float pada;
		glm::vec3 intensity;
		float padb;
	};

	struct SpotLight
	{
		glm::vec3 position;
		float pad;
		glm::mat4 projectionView;
		glm::vec3 direction;
		float FoV;
		glm::vec3 intensity;
		float range;
	};

	struct InstanceData
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

		LightMesh() : vertex_vbo(0),
			element_vbo(0),
			vao(0),
			element_count(0) {}

	};

	LightMesh light_quad_mesh_; // vertex array of vec2 position
	LightMesh light_sphere_mesh_; // element array into vec3 position
	LightMesh light_cone_mesh_; // element array into vec3 position

	void generateLightMeshes();

	void loadShaders();
	void loadMeshes();
	void loadUniformBuffers();

	void geometryPass();
	void ambientPass();
	void directionalLightPass();
	void spotlightPass();
	void pointlightPass();

	void updateBuffers();

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

	struct PerFrameUniforms
	{
		glm::mat4 projectionViewXform;
		glm::vec3 cameraPos;
	};

	struct Material
	{
		glm::vec3 dColour;
		float shininess;
		glm::vec3 sColour;
		float pad;
	};

	//Uniform buffer objects
	GLuint perFrame_ubo_;
	GLuint material_ubo_;
	GLuint pointLight_ubo_;
	GLuint spotLight_ubo_;
	GLuint directionalLight_ubo_;

	//deferred shading stuff
	GLuint gbuffer_position_tex_;
	GLuint gbuffer_normal_tex_;
	GLuint gbuffer_depth_tex_;
	GLuint gbuffer_matColour_tex_;

	GLuint lbuffer_fbo_;
	GLuint lbuffer_colour_rbo_;

	GLuint gbuffer_depth_rbo_;
	GLuint gbuffer_fbo_;

	std::vector<Mesh> meshes_;

	glm::vec3 upDir_;
	glm::mat4 projection_xform_;

	int width_;
	int height_;

	float aspect_ratio_;

	int pointLightCount_;
	int spotLightCount_;
	int directionalLightCount_;
	int materialCount_;



	// VARIABLES FOR SSMA

	bool do_effect = true;
	std::string app_path;

	GLuint albedo_tex;
	GLuint edge_tex;
	GLuint blend_tex;
	GLuint area_tex;
	GLuint search_tex;

	GLuint albedo_fbo;
	GLuint edge_fbo;
	GLuint blend_fbo;

	GLuint edge_shader;
	GLuint blend_shader;
	GLuint neighborhood_shader;

#define INFOLOG_SIZE 4096 * sizeof(GLchar)

	//END VARIABLES FOR SSMA

	//SSMA FUNCTIONS
	void antiAliasingPasses();

	void replace_all(std::string& str, const std::string& from, const std::string& to)
	{
		size_t start_pos = 0;

		while ((start_pos = str.find(from, start_pos)) != std::string::npos)
		{
			str.replace(start_pos, from.length(), to);
			start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
		}
	}

	void create_shader(std::string* vs_text, std::string* ps_text, GLuint* program)
	{
		GLuint shader;
		const GLchar* text_ptr[1];
		GLchar infolog[INFOLOG_SIZE];
		std::string log;

		shader_include(*vs_text);
		replace_all(*vs_text, "hash ", "#");
		shader_include(*ps_text);
		replace_all(*ps_text, "hash ", "#");

		*program = glCreateProgram();

		//VERTEX SHADER
		text_ptr[0] = vs_text->c_str();

		shader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(shader, 1, text_ptr, 0);
		glCompileShader(shader);

		glGetShaderInfoLog(shader, INFOLOG_SIZE, 0, infolog);
		log = infolog;
		std::cerr << log;

		glAttachShader(*program, shader);
		glDeleteShader(shader);

		//PIXEL SHADER
		text_ptr[0] = ps_text->c_str();

		shader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(shader, 1, text_ptr, 0);
		glCompileShader(shader);

		glGetShaderInfoLog(shader, INFOLOG_SIZE, 0, infolog);
		log = infolog;
		std::cerr << log;

		glAttachShader(*program, shader);
		glDeleteShader(shader);

		//LINK
		glLinkProgram(*program);

		glGetProgramInfoLog(*program, INFOLOG_SIZE, 0, infolog);
		log = infolog;
		std::cerr << log;
	}

	void validate_program(GLuint program)
	{
		GLchar infolog[INFOLOG_SIZE];
		std::string log;

		glValidateProgram(program);

		glGetProgramInfoLog(program, INFOLOG_SIZE, 0, infolog);
		log = infolog;
		std::cerr << log;
	}

	void draw_quad()
	{
		glBindVertexArray(light_quad_mesh_.vao);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	}

	void check_fbo()
	{
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			GLenum error = glCheckFramebufferStatus(GL_FRAMEBUFFER);
			std::string serr;
			if (error == GL_FRAMEBUFFER_UNDEFINED){
				serr = "undef";
			}
			if (error == GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT){
				serr = "undef";
			}
			if (error == GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT){
				serr = "undef";
			}
			if (error == GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER){
				serr = "undef";
			}
			if (error == GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE){
				serr = "undef";
			}
			if (error == GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS){
				serr = "undef";
			}
			std::cerr << "FBO not complete.\n";
			exit(1);
		}
	}

	void get_app_path()
	{
		char fullpath[1024];

		/* /proc/self is a symbolic link to the process-ID subdir
		* of /proc, e.g. /proc/4323 when the pid of the process
		* of this program is 4323.
		*
		* Inside /proc/<pid> there is a symbolic link to the
		* executable that is running as this <pid>.  This symbolic
		* link is called "exe".
		*
		* So if we read the path where the symlink /proc/self/exe
		* points to we have the full path of the executable.
		*/

#ifdef __unix__
		int length;
		length = readlink("/proc/self/exe", fullpath, sizeof(fullpath));

		/* Catch some errors: */

		if (length < 0)
		{
			std::cerr << "Couldnt read app path. Error resolving symlink /proc/self/exe.\n";
			the_window.close();
		}

		if (length >= 1024)
		{
			std::cerr << "Couldnt read app path. Path too long. Truncated.\n";
			the_window.close();
		}

		/* I don't know why, but the string this readlink() function
		* returns is appended with a '@'.
		*/
		fullpath[length] = '\0';       /* Strip '@' off the end. */

#endif

#ifdef _WIN32


#endif

		app_path = fullpath;

#ifdef _WIN32
		app_path = app_path.substr(0, app_path.rfind("\\") + 1);
		//when the exe is located in {source}/build/Debug/smaa.exe and we need the {source}
		app_path += "../../";
#endif

#ifdef __unix__
		app_path = app_path.substr(0, app_path.rfind("/") + 1);
		//when the exe is located in {source}/build/smaa and we need the {source}
		app_path += "../";
#endif
	}

	void get_opengl_error(bool ignore = false)
	{
		bool got_error = false;
		GLenum error = 0;
		error = glGetError();
		std::string errorstring = "";

		while (error != GL_NO_ERROR)
		{
			if (error == GL_INVALID_ENUM)
			{
				//An unacceptable value is specified for an enumerated argument. The offending command is ignored and has no other side effect than to set the error flag.
				errorstring += "OpenGL error: invalid enum...\n";
				got_error = true;
			}

			if (error == GL_INVALID_VALUE)
			{
				//A numeric argument is out of range. The offending command is ignored and has no other side effect than to set the error flag.
				errorstring += "OpenGL error: invalid value...\n";
				got_error = true;
			}

			if (error == GL_INVALID_OPERATION)
			{
				//The specified operation is not allowed in the current state. The offending command is ignored and has no other side effect than to set the error flag.
				errorstring += "OpenGL error: invalid operation...\n";
				got_error = true;
			}

			if (error == GL_STACK_OVERFLOW)
			{
				//This command would cause a stack overflow. The offending command is ignored and has no other side effect than to set the error flag.
				errorstring += "OpenGL error: stack overflow...\n";
				got_error = true;
			}

			if (error == GL_STACK_UNDERFLOW)
			{
				//This command would cause a stack underflow. The offending command is ignored and has no other side effect than to set the error flag.
				errorstring += "OpenGL error: stack underflow...\n";
				got_error = true;
			}

			if (error == GL_OUT_OF_MEMORY)
			{
				//There is not enough memory left to execute the command. The state of the GL is undefined, except for the state of the error flags, after this error is recorded.
				errorstring += "OpenGL error: out of memory...\n";
				got_error = true;
			}



			error = glGetError();
		}

		if (got_error && !ignore)
		{
			std::cerr << errorstring;

			return;
		}
	}

	void shader_include(std::string& shader)
	{
		size_t start_pos = 0;
		std::string include_dir = "#include ";

		while ((start_pos = shader.find(include_dir, start_pos)) != std::string::npos)
		{
			int pos = start_pos + include_dir.length() + 1;
			int length = shader.find("\"", pos);
			std::string file = shader.substr(pos, length - pos);
			std::string content = "";

			std::ifstream f;
			f.open((app_path + file).c_str());

			if (f.is_open())
			{
				char buffer[1024];

				while (!f.eof())
				{
					f.getline(buffer, 1024);
					content += buffer;
					content += "\n";
				}
			}
			else
			{
				std::cerr << "Couldn't include shader file: " << app_path + file << "\n";
			}

			shader.replace(start_pos, (length + 1) - start_pos, content);
			start_pos += content.length();
		}
	}
	//END SSMA FUNCTIONS


};
