/*!
@file    glapp.cpp
@author  pghali@digipen.edu
@co-author brandonjunjie.ho@digipen.edu
@date    5/10/2023

This file implements functionality useful and necessary to build OpenGL
applications including use of external APIs such as GLFW to create a
window and start up an OpenGL context and to extract function pointers
to OpenGL implementations.

*//*__________________________________________________________________________*/

/*                                                                   includes
----------------------------------------------------------------------------- */
#include <glapp.h>
#include <glhelper.h>
#include <glm/glm.hpp>
#include <vector>
#include <array>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>

/*                                                   objects with file scope
----------------------------------------------------------------------------- */
GLApp::GLModel GLApp::mdl{};

double previousTime = glfwGetTime();
double fps = 0.0;

/*  _________________________________________________________________________ */
/*! init
@param none
@return none

Clears the color buffer, sets the viewport as the entire window and initialize
VAO and create shader program
*/
void GLApp::init() {

	// Part 1: clear colorbuffer with RGBA value in glClearColor ...
	glClearColor(1.f, 0.f, 0.f, 1.f);

	// Part 2: use entire window as viewport ...
	glViewport(0, 0, GLHelper::width, GLHelper::height);

	// Part 3: initialize VAO and create shader program
	mdl.setup_vao();
	mdl.setup_shdrpgm();
}

/*  _________________________________________________________________________ */
/*! update
@param none
@return none

Changes the color buffer with linear interpolation between two colors.
*/
void GLApp::update() {
	
	std::vector<GLclampf> color1{ 1.f, 0.f, 0.f };
	std::vector<GLclampf> color2{ 0.f, 1.f, 1.f };
	std::vector<GLclampf> color_value(3);

	float elapsedTime = static_cast<float>(glfwGetTime());
	elapsedTime = std::cosf(elapsedTime) * 0.5f + 0.5f;	

	color_value[0] = color1[0] + (color2[0] - color1[0]) * elapsedTime;
	color_value[1] = color1[1] + (color2[1] - color1[1]) * elapsedTime;
	color_value[2] = color1[2] + (color2[2] - color1[2]) * elapsedTime;

	glClearColor(color_value[0], color_value[1], color_value[2], 1.f);

}

/*  _________________________________________________________________________ */
/*! draw
@param none
@return none

Renders the objects onto the back buffer and sets the title of the window.
*/
void GLApp::draw() {
	
	// clear buffer with color set in GLApp::init()
	glClear(GL_COLOR_BUFFER_BIT);

	// now, render rectangular model from NDC coordinates to viewport
	mdl.draw();
	
	// window title
	std::stringstream title;

	title << "Tutorial 1 | Brandon Ho Jun Jie | " << std::setprecision(2) << std::fixed << GLHelper::fps;
	
	glfwSetWindowTitle(GLHelper::ptr_window, title.str().c_str());
}

/*  _________________________________________________________________________ */
/*! cleanup
@param none
@return none

Empty
*/
void GLApp::cleanup() {
  // empty for now
}

/*  _________________________________________________________________________ */
/*! setup_vao
@param none
@return none

Set up a Vertex Array Object (VAO) that contains vertex position and color data for rendering.
Creates a Vertex Buffer Object (VBO) and stores the vertex position and color data in it.
Creates a VAO and binds the VBO to it, contains information on how to handle the data.
Creates an Element Buffer Object (EBO) to specify the order in which the vertices should be rendered.
Binds the EBO to the VAO.
*/
void GLApp::GLModel::setup_vao() {
	// Define vertex position and color attributes
	std::array<glm::vec2, 4> pos_vtx{
	glm::vec2(0.5f, -0.5f), glm::vec2(0.5f, 0.5f),
	glm::vec2(-0.5f, 0.5f), glm::vec2(-0.5f, -0.5f)
	};
	std::array<glm::vec3, 4> clr_vtx{
	glm::vec3(1.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f),
	glm::vec3(0.f, 0.f, 1.f), glm::vec3(1.f, 1.f, 1.f)
	};

	// transfer vertex position and color attributes to VBO
	GLuint vbo_hdl;
	glCreateBuffers(1, &vbo_hdl);
	glNamedBufferStorage(vbo_hdl,
		sizeof(glm::vec2) * pos_vtx.size() + sizeof(glm::vec3) * clr_vtx.size(),
		nullptr, GL_DYNAMIC_STORAGE_BIT);
	glNamedBufferSubData(vbo_hdl, 0,
		sizeof(glm::vec2) * pos_vtx.size(), pos_vtx.data());
	glNamedBufferSubData(vbo_hdl, sizeof(glm::vec2) * pos_vtx.size(),
		sizeof(glm::vec3) * clr_vtx.size(), clr_vtx.data());

	// encapsulate information about contents of VBO and VBO handle
	// to another object called VAO
	glCreateVertexArrays(1, &vaoid);

	// for vertex position array, we use vertex attribute index 8
	// and vertex buffer binding point 3
	glEnableVertexArrayAttrib(vaoid, 8);
	glVertexArrayVertexBuffer(vaoid, 3, vbo_hdl, 0, sizeof(glm::vec2));
	glVertexArrayAttribFormat(vaoid, 8, 2, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribBinding(vaoid, 8, 3);

	// for vertex color array, we use vertex attribute index 9
	// and vertex buffer binding point 4
	glEnableVertexArrayAttrib(vaoid, 9);
	glVertexArrayVertexBuffer(vaoid, 4, vbo_hdl,
		sizeof(glm::vec2) * pos_vtx.size(), sizeof(glm::vec3));
	glVertexArrayAttribFormat(vaoid, 9, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribBinding(vaoid, 9, 4);
	primitive_type = GL_TRIANGLES;
	std::array<GLushort, 6> idx_vtx{ 0, 1, 2, 2, 3, 0 };
	idx_elem_cnt = idx_vtx.size();
	GLuint ebo_hdl;
	glCreateBuffers(1, &ebo_hdl);
	glNamedBufferStorage(ebo_hdl, sizeof(GLushort) * idx_elem_cnt,
		reinterpret_cast<GLvoid*>(idx_vtx.data()),
		GL_DYNAMIC_STORAGE_BIT);
	glVertexArrayElementBuffer(vaoid, ebo_hdl);
	glBindVertexArray(0);
}

/*  _________________________________________________________________________ */
/*! update
@param none
@return none

Compiles the vertex shader and fragment shader and links them to a shader program.
*/
void GLApp::GLModel::setup_shdrpgm() {
	std::vector<std::pair<GLenum, std::string>> shdr_files;
	shdr_files.emplace_back(std::make_pair(
		GL_VERTEX_SHADER,
		"../shaders/my-tutorial-1.vert"));
	shdr_files.emplace_back(std::make_pair(
		GL_FRAGMENT_SHADER,
		"../shaders/my-tutorial-1.frag"));
	shdr_pgm.CompileLinkValidate(shdr_files);
	if (GL_FALSE == shdr_pgm.IsLinked()) {
		std::cout << "Unable to compile/link/validate shader programs" << "\n";
		std::cout << shdr_pgm.GetLog() << std::endl;
		std::exit(EXIT_FAILURE);
	}
}

/*  _________________________________________________________________________ */
/*! update
@param none
@return none

Uses the shader program and vertex array object (VAO) to draw geometry on the screen.
*/
void GLApp::GLModel::draw() {

	// there are many shader programs initialized - here we're saying
	// which specific shader program should be used to render geometry
	shdr_pgm.Use();

	// there are many models, each with their own initialized VAO object
	// here, we're saying which VAO's state should be used to set up pipe
	glBindVertexArray(vaoid);

	// here, we're saying what primitive is to be rendered and how many
	// such primitives exist.
	// the graphics driver knows where to get the indices because the VAO
	// containing this state information has been made current ...

	glDrawElements(primitive_type, idx_elem_cnt, GL_UNSIGNED_SHORT, NULL);
	// after completing the rendering, we tell the driver that VAO
	// vaoid and current shader program are no longer current

	glBindVertexArray(0);
	shdr_pgm.UnUse();
}
