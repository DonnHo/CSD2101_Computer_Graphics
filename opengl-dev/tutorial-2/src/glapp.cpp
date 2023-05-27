/*!
* @file    glapp.cpp
* @author  pghali@digipen.edu
* @co-author brandonjunjie.ho@digipen.edu
* @date    5/17/2023
*
* @brief This file implements functionality useful and necessary to build OpenGL
*		 applications including use of external APIs such as GLFW to create a
*		 window and start up an OpenGL context and to extract function pointers
*		 to OpenGL implementations. The functions also divide the viewport into 
*		 4 sections and render different primitives into their respective viewports.
*
*        **BONUS TASK** is implemented in function
*		 GLApp::tristrip_model and GLApp::GLModel::draw.
*		 Lines: 239 - 241, 493 - 495, 508 - 512
*//*__________________________________________________________________________*/

/*                                                                   includes
----------------------------------------------------------------------------- */
#include <glapp.h>
#include <glhelper.h>
#include <glm/glm.hpp>
#include <iostream>
#include <sstream>
#include <iomanip>

/*                                                   objects with file scope
----------------------------------------------------------------------------- */
std::vector<GLApp::GLModel> GLApp::models{};
std::vector<GLApp::GLViewport> GLApp::vps{};

/*  _________________________________________________________________________ */
/*! GLApp::init
 * @brief Initialize the GLApp.
 *
 * This function initializes the GLApp by performing the following tasks:
 * 1. Clears the color buffer to white with RGBA value in glClearColor.
 * 2. Splits the color buffer into four viewports: top-left, top-right,
 *    bottom-left, and bottom-right.
 * 3. Creates different geometries and inserts them into the GLApp::models repository container.
 *
 * @param none
 * @return void
*/
void GLApp::init() {

	// Part 1: clear colorbuffer to white with RGBA value in glClearColor ...
	glClearColor(1.f, 1.f, 1.f, 1.f);

	// Part 2: split color buffer into four viewports
	GLint w{ GLHelper::width }, h{ GLHelper::height };

	// top-left viewport
	vps.emplace_back(GLApp::GLViewport{ 0, h / 2, w / 2, h / 2 });
	// top-right viewport
	vps.emplace_back(GLApp::GLViewport{ w / 2, h / 2, w / 2, h / 2 });
	// bottom-left viewport
	vps.emplace_back(GLApp::GLViewport{ 0, 0, w / 2, h / 2 });
	// bottom-right viewport
	vps.emplace_back(GLApp::GLViewport{ w / 2, 0, w / 2, h / 2 });

	// Part 3: create different geometries and insert them into
	// repositor container GLApp::models
	GLApp::models.emplace_back(points_model(20, 20,
											"../shaders/my-tutorial-2.vert",
										    "../shaders/my-tutorial-2.frag"));

	GLApp::models.emplace_back(lines_model(40, 40, 
											"../shaders/my-tutorial-2.vert",
											"../shaders/my-tutorial-2.frag"));

	GLApp::models.emplace_back(trifans_model(50, 
											"../shaders/my-tutorial-2.vert",
											"../shaders/my-tutorial-2.frag"));

	GLApp::models.emplace_back(tristrip_model(10, 15,
											"../shaders/my-tutorial-2.vert",
											"../shaders/my-tutorial-2.frag"));
}

/*  _________________________________________________________________________ */
/*! GLApp::update
 * @brief Update the GLApp.
 *
 * This function updates the GLApp by clearing the color buffer to white with RGBA
 * value in glClearColor.
 *
 * @param none
 * @return void
*/
void GLApp::update() {

	glClearColor(1.f, 1.f, 1.f, 1.f);

}

/*  _________________________________________________________________________ */
/*! GLApp::draw
 * @brief Draw the GLApp.
 *
 * This function draws the GLApp by performing the following tasks:
 * 1. Sets the window title with information about the number of primitives and draw counts.
 * 2. Clears the back buffer.
 * 3. Renders points in the top-left viewport.
 * 4. Renders lines in the top-right viewport.
 * 5. Renders triangle fan in the bottom-left viewport.
 * 6. Renders triangle strip in the bottom-right viewport.
 *
 * @param none
 * @return void
*/
void GLApp::draw() {

	// window title
	std::stringstream title;

	title << "Tutorial 2 | Brandon Ho Jun Jie | " << "POINTS: " << GLApp::models[0].primitive_cnt	<< ", "
																<< GLApp::models[0].draw_cnt		<< " | "
												  << "LINES: "  << GLApp::models[1].primitive_cnt	<< ", "
																<< GLApp::models[1].draw_cnt		<< " | "
												  << "FAN: "	<< GLApp::models[2].primitive_cnt	<< ", "
																<< GLApp::models[2].draw_cnt		<< " | " 
												  << "STRIP: "  << GLApp::models[3].primitive_cnt	<< ", "
																<< GLApp::models[3].draw_cnt		<< " | " 
												  << std::setprecision(2) << std::fixed << GLHelper::fps;
	
	glfwSetWindowTitle(GLHelper::ptr_window, title.str().c_str());

	// clear back buffer
	glClear(GL_COLOR_BUFFER_BIT);

	// render points in top-left viewport
	glViewport(vps[0].x, vps[0].y, vps[0].width, vps[0].height);
	GLApp::models[0].draw();

	// render lines in top-right viewport
	glViewport(vps[1].x, vps[1].y, vps[1].width, vps[1].height);
	GLApp::models[1].draw();

	// render triangle fan in bottom-left viewport
	glViewport(vps[2].x, vps[2].y, vps[2].width, vps[2].height);
	GLApp::models[2].draw();

	// render triangle strip in bottom-right viewport
	glViewport(vps[3].x, vps[3].y, vps[3].width, vps[3].height);
	GLApp::models[3].draw();
}

/*  _________________________________________________________________________ */
/*! GLApp::cleanup
 * @brief Empty.
 * 
 * @param none
 * @return none
*/
void GLApp::cleanup()
{
  // empty for now
}

/*  _________________________________________________________________________ */
/*! GLApp::GLModel::setup_shdrpgm
 * @brief Setup the shader program for the GLModel.
 *
 * This function sets up the shader program for the GLModel by performing the following tasks:
 * 1. Creates a vector of shader files with their respective types.
 * 2. Compiles, links, and validates the shader program.
 * 3. If the shader program fails to link, an error message is printed and the program exits with a failure status.
 *
 * @param[in] vtx_shader The file path of the vertex shader.
 * @param[in] frg_shader The file path of the fragment shader.
 * @return void
*/
void GLApp::GLModel::setup_shdrpgm(std::string vtx_shader,
								   std::string frg_shader) {
	std::vector<std::pair<GLenum, std::string>> shdr_files;
	shdr_files.emplace_back(std::make_pair(GL_VERTEX_SHADER, vtx_shader));
	shdr_files.emplace_back(std::make_pair(GL_FRAGMENT_SHADER, frg_shader));
	shdr_pgm.CompileLinkValidate(shdr_files);
	
	if (GL_FALSE == shdr_pgm.IsLinked())
	{
		std::cout << "Unable to compile/link/validate shader programs\n";
		std::cout << shdr_pgm.GetLog() << "\n";
		std::exit(EXIT_FAILURE);
	}
}

/*  _________________________________________________________________________ */
/*! GLApp::GLModel::draw
 * @brief Draw the GLModel.
 *
 * This function draws the GLModel by performing the following tasks:
 * 1. Sets the current shader program for rendering.
 * 2. Binds the Vertex Array Object (VAO) to set up the pipeline state.
 * 3. Specifies the primitive type and the number of primitives to be rendered.
 *    - For GL_POINTS: Sets the point size, vertex attribute, and calls glDrawArrays.
 *    - For GL_LINES: Sets the line width, vertex attribute, and calls glDrawArrays.
 *    - For GL_TRIANGLE_FAN: Calls glDrawArrays.
 *    - For GL_TRIANGLE_STRIP: Enables primitive restart, calls glDrawElements, and disables primitive restart.
 * 4. Unbinds the VAO and sets the current shader program to no longer be current.
 *
 * @param none
 * @return void
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

	switch(primitive_type)
	{
	case GL_POINTS:
		glPointSize(10.f);
		glVertexAttrib3f(1, 1.f, 0.f, 0.f); // redcolor for points
		glDrawArrays(primitive_type, 0, draw_cnt);
		glPointSize(1.f);
		break;
	case GL_LINES:
		glLineWidth(3.f);
		glVertexAttrib3f(1, 0.f, 0.f, 1.f); // blue color for lines
		glDrawArrays(primitive_type, 0, draw_cnt);
		glLineWidth(1.f);
		break;
	case GL_TRIANGLE_FAN:
		glDrawArrays(primitive_type, 0, draw_cnt);
		break;
	case GL_TRIANGLE_STRIP:
		glEnable(GL_PRIMITIVE_RESTART);
		glDrawElements(primitive_type, draw_cnt, GL_UNSIGNED_SHORT, NULL);
		glDisable(GL_PRIMITIVE_RESTART);
		break;
	}
	// after completing the rendering, we tell the driver that VAO
	// vaoid and current shader program are no longer current

	glBindVertexArray(0);
	shdr_pgm.UnUse();
}

/*  _________________________________________________________________________ */
/*! GLApp::points_model
 * @brief Create a GLModel for rendering points.
 *
 * This function creates a GLModel for rendering points by performing the following tasks:
 * 1. Calculates the positions of the vertices based on the number of slices and stacks.
 * 2. Creates a Vertex Buffer Object (VBO) and Vertex Array Object (VAO) to store the vertex data.
 * 3. Sets up the vertex attribute and format for the VAO.
 * 4. Initializes the GLModel's attributes, including the VAO, primitive type, shader program, and draw count.
 * 5. Returns the created GLModel.
 *
 * @param[in] slices The number of slices.
 * @param[in] stacks The number of stacks.
 * @param[in] vtx_shdr The vertex shader file path.
 * @param[in] frg_shdr The fragment shader file path.
 * @return The created GLModel for rendering points.
*/
GLApp::GLModel GLApp::points_model(int slices, int stacks,
					  std::string vtx_shdr, std::string frg_shdr)
{
	std::vector<glm::vec2> pos_vtx((slices + 1) * (stacks + 1));
	float xinterval{ 2.f / slices };
	float yinterval{ 2.f / stacks };

	for (int j{}; j < stacks + 1; ++j)
	{
		for (int i{}; i < slices + 1; ++i)
		{
			pos_vtx[i + j * (slices + 1)] = glm::vec2{ xinterval * i - 1.f, yinterval * j - 1.f };
		}
	}

	// define VAO handle
	GLuint vbo_hdl;
	glCreateBuffers(1, &vbo_hdl);
	glNamedBufferStorage(vbo_hdl, sizeof(glm::vec2) * pos_vtx.size(),
						 pos_vtx.data(), GL_DYNAMIC_STORAGE_BIT);
	GLuint vaoid;
	glCreateVertexArrays(1, &vaoid);
	glEnableVertexArrayAttrib(vaoid, 0);
	glVertexArrayVertexBuffer(vaoid, 0, vbo_hdl, 0, sizeof(glm::vec2));
	glVertexArrayAttribFormat(vaoid, 0, 2, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribBinding(vaoid, 0, 0);
	glBindVertexArray(0);

	// assign attributes to instance model to return
	GLModel mdl{};
	mdl.vaoid = vaoid;
	mdl.primitive_type = GL_POINTS;
	mdl.setup_shdrpgm(vtx_shdr, frg_shdr);
	mdl.draw_cnt = pos_vtx.size();
	mdl.primitive_cnt = mdl.draw_cnt;

	return mdl;
}

/*  _________________________________________________________________________ */
/*! GLApp::lines_model
 * @brief Create a GLModel for rendering lines.
 *
 * This function creates a GLModel for rendering lines by performing the following tasks:
 * 1. Calculates the positions of the vertices based on the number of slices and stacks.
 * 2. Creates a Vertex Buffer Object (VBO) and Vertex Array Object (VAO) to store the vertex data.
 * 3. Sets up the vertex attribute and format for the VAO.
 * 4. Initializes the GLModel's attributes, including the VAO, primitive type, shader program, and draw count.
 * 5. Returns the created GLModel.
 *
 * @param[in] slices The number of slices.
 * @param[in] stacks The number of stacks.
 * @param[in] vtx_shdr The vertex shader file path.
 * @param[in] frg_shdr The fragment shader file path.
 * @return The created GLModel for rendering lines.
*/
GLApp::GLModel GLApp::lines_model(int slices, int stacks,
					  std::string vtx_shdr, std::string frg_shdr)
{
	// count of vertices need for slices and stacks
	int const countx{ (slices + 1) * 2 };
	int const county{ (stacks + 1) * 2 };
	std::vector<glm::vec2> pos_vtx(countx * county);

	// interval between slices and stacks
	float const u{ 2.f / static_cast<float>(slices) };
	float const m{ 2.f / static_cast<float>(stacks) };
	int index{ 0 };

	for (int col{ 0 }; col <= slices; ++col) {
		float x{ u * static_cast<float>(col) - 1.f };
		pos_vtx[index++] = glm::vec2(x, -1.f);
		pos_vtx[index++] = glm::vec2(x, 1.f);
	}
	for (int row{ 0 }; row <= stacks; ++row)
	{
		float y{ m * static_cast<float>(row) - 1.f };
		pos_vtx[index++] = glm::vec2(-1.f, y);
		pos_vtx[index++] = glm::vec2(1.f, y);
	}
	// compute and store endpoints for (slices+1) set of lines
	// for each x from -1 to 1
	// start endpoint is (x, -1) and end endpoint is (x, 1)
	// set up VAO as in GLApp::points_model

	// define VAO handle
	GLuint vbo_hdl;
	glCreateBuffers(1, &vbo_hdl);
	glNamedBufferStorage(vbo_hdl, sizeof(glm::vec2) * pos_vtx.size(),
		pos_vtx.data(), GL_DYNAMIC_STORAGE_BIT);

	GLuint vaoid;
	glCreateVertexArrays(1, &vaoid);
	glEnableVertexArrayAttrib(vaoid, 0);
	glVertexArrayVertexBuffer(vaoid, 0, vbo_hdl, 0, sizeof(glm::vec2));
	glVertexArrayAttribFormat(vaoid, 0, 2, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribBinding(vaoid, 0, 0);
	glBindVertexArray(0);

	GLApp::GLModel mdl{};
	mdl.vaoid = vaoid; // set up VAO same as in GLApp::points_model
	mdl.primitive_type = GL_LINES;
	mdl.setup_shdrpgm(vtx_shdr, frg_shdr);
	mdl.draw_cnt = 2 * (slices + 1) + 2 * (stacks + 1); // number of vertices
	mdl.primitive_cnt = mdl.draw_cnt / 2; // number of primitives (not used)
	return mdl;
}

/*  _________________________________________________________________________ */
/*! GLApp::trifans_model
 * @brief Create a GLModel for rendering triangle fans.
 *
 * This function creates a GLModel for rendering triangle fans by performing the following tasks:
 * 1. Generates the positions of the vertices based on the number of slices to form a triangle fan.
 * 2. Computes random color coordinates for each vertex.
 * 3. Creates a Vertex Buffer Object (VBO) and Vertex Array Object (VAO) to store the vertex data.
 * 4. Sets up the vertex attributes and formats for the VAO.
 * 5. Initializes the GLModel's attributes, including the VAO, primitive type, shader program, and draw count.
 * 6. Returns the created GLModel.
 *
 * @param[in] slices The number of slices.
 * @param[in] vtx_shdr The vertex shader file path.
 * @param[in] frg_shdr The fragment shader file path.
 * @return The created GLModel for rendering triangle fans.
*/
GLApp::GLModel GLApp::trifans_model(int slices, std::string vtx_shdr,
									std::string frg_shdr)
{
	// Step 1: Generate the (slices+2) count of vertices required to
	// render a triangle fan parameterization of a circle with unit
	// radius and centered at (0, 0)
	std::vector<glm::vec2> pos_vtx(slices + 2);

	float interval{ glm::radians(360.f / slices)};

	pos_vtx[0] = glm::vec2(0.f, 0.f);

	for (int i{ 0 }; i <= slices; ++i)
	{
		pos_vtx[i+1] = glm::vec2(cosf(i * interval), sinf(i * interval));
	}

	// Step 2: In addition to vertex position coordinates, compute
	// (slices+2) count of vertex color coordinates.
	// Each RGB component must be randomly computed.
	std::vector<glm::vec3> clr_vtx(slices + 2);
	
	for (glm::vec3& tmp : clr_vtx)
	{
		tmp = glm::vec3(static_cast<float>(rand()) / RAND_MAX, static_cast<float>(rand()) / RAND_MAX, static_cast<float>(rand()) / RAND_MAX);
	}

	// Step 3: Generate a VAO handle to encapsulate the VBO(s) and
	// state of this triangle mesh
	// define VAO handle
	GLuint vbo_hdl;
	glCreateBuffers(1, &vbo_hdl);
	glNamedBufferStorage(vbo_hdl, sizeof(glm::vec2) * pos_vtx.size() + sizeof(glm::vec3) * clr_vtx.size(),
	nullptr, GL_DYNAMIC_STORAGE_BIT);
	glNamedBufferSubData(vbo_hdl, 0,
		sizeof(glm::vec2) * pos_vtx.size(), pos_vtx.data());
	glNamedBufferSubData(vbo_hdl, sizeof(glm::vec2) * pos_vtx.size(),
		sizeof(glm::vec3) * clr_vtx.size(), clr_vtx.data());

	GLuint vaoid;
	glCreateVertexArrays(1, &vaoid);
	glEnableVertexArrayAttrib(vaoid, 0);
	glVertexArrayVertexBuffer(vaoid, 0, vbo_hdl, 0, sizeof(glm::vec2));
	glVertexArrayAttribFormat(vaoid, 0, 2, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribBinding(vaoid, 0, 0);

	glEnableVertexArrayAttrib(vaoid, 1);
	glVertexArrayVertexBuffer(vaoid, 1, vbo_hdl,
		sizeof(glm::vec2) * pos_vtx.size(), sizeof(glm::vec3));
	glVertexArrayAttribFormat(vaoid, 1, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribBinding(vaoid, 1, 1);

	glBindVertexArray(0);

	GLApp::GLModel mdl{};
	mdl.vaoid = vaoid; // set up VAO same as in GLApp::points_model
	mdl.primitive_type = GL_TRIANGLE_FAN;
	mdl.setup_shdrpgm(vtx_shdr, frg_shdr);
	mdl.draw_cnt = pos_vtx.size(); // number of vertices
	mdl.primitive_cnt = slices; // number of primitives (not used)

	// Step 4: Return an appropriately initialized instance of GLApp::GLModel
	return mdl;
}

/*  _________________________________________________________________________ */
/*! GLApp::tristrip_model
 * @brief Create a GLModel for rendering triangle strips.
 *
 * This function creates a GLModel for rendering triangle strips by performing the following tasks:
 * 1. Generates the positions of the vertices based on the number of slices and stacks.
 * 2. Generates the indices for rendering the triangle strips.
 * 3. Computes random color coordinates for each vertex.
 * 4. Creates a Vertex Buffer Object (VBO) and Element Buffer Object (EBO) to store the vertex and index data.
 * 5. Creates a Vertex Array Object (VAO) and sets up the vertex attributes and formats for the VAO.
 * 6. Initializes the GLModel's attributes, including the VAO, primitive type, shader program, and draw count.
 * 7. Returns the created GLModel.
 *
 * @param slices The number of slices.
 * @param stacks The number of stacks.
 * @param vtx_shdr The vertex shader file path.
 * @param frg_shdr The fragment shader file path.
 * @return The created GLModel for rendering triangle strips.
*/
GLApp::GLModel GLApp::tristrip_model(int slices, int stacks,
									 std::string vtx_shdr,
									 std::string frg_shdr)
{
	std::vector<glm::vec2> pos_vtx((slices + 1) * (stacks + 1));
	float xinterval{ 2.f / slices };
	float yinterval{ 2.f / stacks };

	for (int j{}; j < stacks + 1; ++j)
	{
		for (int i{}; i < slices + 1; ++i)
		{
			pos_vtx[i + j * (slices + 1)] = glm::vec2{ xinterval * i - 1.f, yinterval * j - 1.f };
		}
	}

	// Bonus task
	GLushort primitiveRestartIndex = std::numeric_limits<GLushort>::max();
	glPrimitiveRestartIndex(primitiveRestartIndex);

	std::vector<GLushort> idx_vtx((slices + 1) * 2 * stacks + 1 * (stacks - 1));

	int index{ 0 };
	for (int row{ 0 }; row < stacks; ++row)
	{
		for (int col{ 0 }; col < slices + 1; ++col)
		{
			idx_vtx[index++] = static_cast<GLushort>(col + (slices + 1) * (row + 1));
			idx_vtx[index++] = static_cast<GLushort>(col + (slices + 1) * row);
		}

		// Bonus task
		if (row < stacks - 1)
		{
			idx_vtx[index++] = primitiveRestartIndex;
		}
	}

	// Step 2: In addition to vertex position coordinates, compute
	// (slices + 1) * (stacks + 1) count of vertex color coordinates.
	// Each RGB component must be randomly computed.
	std::vector<glm::vec3> clr_vtx((slices + 1) * (stacks + 1));


	for (glm::vec3& tmp : clr_vtx)
	{
		tmp = glm::vec3(static_cast<float>(rand()) / RAND_MAX, static_cast<float>(rand()) / RAND_MAX, static_cast<float>(rand()) / RAND_MAX);
	}

	// Step 3: Generate a VAO handle to encapsulate the VBO(s) and
	// state of this triangle mesh
			// define VAO handle
	GLuint vbo_hdl;
	glCreateBuffers(1, &vbo_hdl);
	glNamedBufferStorage(vbo_hdl, sizeof(glm::vec2) * pos_vtx.size() + sizeof(glm::vec3) * clr_vtx.size(),
		nullptr, GL_DYNAMIC_STORAGE_BIT);
	glNamedBufferSubData(vbo_hdl, 0,
		sizeof(glm::vec2) * pos_vtx.size(), pos_vtx.data());
	glNamedBufferSubData(vbo_hdl, sizeof(glm::vec2) * pos_vtx.size(),
		sizeof(glm::vec3) * clr_vtx.size(), clr_vtx.data());

	GLuint vaoid;
	glCreateVertexArrays(1, &vaoid);
	glEnableVertexArrayAttrib(vaoid, 0);
	glVertexArrayVertexBuffer(vaoid, 0, vbo_hdl, 0, sizeof(glm::vec2));
	glVertexArrayAttribFormat(vaoid, 0, 2, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribBinding(vaoid, 0, 0);

	glEnableVertexArrayAttrib(vaoid, 1);
	glVertexArrayVertexBuffer(vaoid, 1, vbo_hdl,
		sizeof(glm::vec2) * pos_vtx.size(), sizeof(glm::vec3));
	glVertexArrayAttribFormat(vaoid, 1, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribBinding(vaoid, 1, 1);

	GLuint ebo_hdl;
	glCreateBuffers(1, &ebo_hdl);
	glNamedBufferStorage(ebo_hdl, sizeof(GLushort) * idx_vtx.size(),
		reinterpret_cast<GLvoid*>(idx_vtx.data()),
		GL_DYNAMIC_STORAGE_BIT);
	glVertexArrayElementBuffer(vaoid, ebo_hdl);
	glBindVertexArray(0);

	GLApp::GLModel mdl{};
	mdl.vaoid = vaoid; // set up VAO same as in GLApp::points_model
	mdl.primitive_type = GL_TRIANGLE_STRIP;
	mdl.setup_shdrpgm(vtx_shdr, frg_shdr);
	mdl.draw_cnt = idx_vtx.size(); // number of vertices
	mdl.primitive_cnt = (slices * stacks * 2); // number of primitives (not used)

	// Step 4: Return an appropriately initialized instance of GLApp::GLModel
	return mdl;
}
