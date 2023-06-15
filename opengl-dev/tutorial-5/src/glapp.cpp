/*!
* @file    glapp.cpp
* @author  pghali@digipen.edu
* @co-author brandonjunjie.ho@digipen.edu
* @date    6/10/2023
*
* @brief This file implements functionality useful and necessary to build OpenGL
*		 applications including use of external APIs such as GLFW to create a
*		 window and start up an OpenGL context and to extract function pointers
*		 to OpenGL implementations. The context contains the implementation of
*        texture mapping, ease-in/out animation, blending modes for transparency
*		 and different texture sampling modes.
*
* 		 Extra features: TaskID goes up to 8, additional 2 tasks, mainly done in
*						 fragment shader. Line 111 - 174.
* 
*						 - Task 7 implements special effects using the fragment
*						 shader and renders out a cardioid shape with movement
*						 and colors.
*						
*						 - Task 8 implements rotating tunnel effect by mapping
*						   uv coordinates to a circular space and sampling a
*                          texture based on the modified coordinates.
*						   Note: extra texture used "../images/water-rgba-256.tex"
*//*__________________________________________________________________________*/

/*                                                                   includes
----------------------------------------------------------------------------- */
#include <glapp.h>
#include <glhelper.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <glm/gtc/type_ptr.inl> // for glm::value_ptr
#include <glm/gtc/constants.hpp>    // for glm::pi

/*                                                   objects with file scope
----------------------------------------------------------------------------- */
GLApp::GLModel GLApp::mdl{};

GLuint GLApp::taskID{ 0 };

// flag variables
GLboolean GLApp::taskFlag{ false };
GLboolean GLApp::modFlag{ false };
GLboolean GLApp::modFlagTriggered{ false };
GLboolean GLApp::alphaFlag{ false };
GLboolean GLApp::alphaFlagTriggered{ false };

// Bonus task variables
GLfloat GLApp::animTime{ 30.f };
GLfloat GLApp::animElapsedTime{ 0.f };

// tileSize for task 3
GLfloat GLApp::tileSize{ 0.f };

// texture variables
GLuint GLApp::texobj1{};
GLuint GLApp::texobj2{};

/*  _________________________________________________________________________ */
/*! GLApp::init
 * @brief Initialize the GLApp.
 *
 * This function initializes the GLApp by performing the following tasks:
 * 1. Clears the color buffer to white using glClearColor.
 * 2. Sets the viewport to use the entire window.
 * 3. Sets up VAO object and shader program.
 * 4. Sets up texture objects.
 *
 * @param none
 * @return void
*/
void GLApp::init() {

	// Part 1: clear colorbuffer with RGBA value in glClearColor ...
	glClearColor(1.f, 1.f, 1.f, 1.f);

	// Part 2: use entire window as viewport ...
	glViewport(0, 0, GLHelper::width, GLHelper::height);

	// Part 3: initialize VAO and create shader program
	mdl.setup_vao();
	mdl.setup_shdrpgm();

	texobj1 = setup_texobj("../images/duck-rgba-256.tex");
	texobj2 = setup_texobj("../images/water-rgba-256.tex");
}

/*  _________________________________________________________________________ */
/*! GLApp::update
 * @brief Update the GLApp.
 *
 * This function updates the GLApp by performing the following tasks:
 * 1. Update user input and set flags
 * 2. Update elapsed time for animation and reset when not in use.
 * 3. Implement ease in/out animation for change in tileSize
 *
 * @param none
 * @return void
*/
void GLApp::update()
{
	// update user input and set flags
	if (GLHelper::keystateT)
	{
		if (!taskFlag)
		{
			// increment taskID from 0 - 7;
			taskFlag = GL_TRUE;
			taskID = taskID < 8 ? taskID + 1 : 0;
		}
	}
	else
	{
		taskFlag = GL_FALSE;
	}

	if (GLHelper::keystateM && !modFlagTriggered)
	{
		modFlagTriggered = GL_TRUE;
		modFlag = ~modFlag;
	}
	
	if(!GLHelper::keystateM)
	{
		modFlagTriggered = GL_FALSE;
	}

	if (GLHelper::keystateA && !alphaFlagTriggered)
	{
		alphaFlagTriggered = GL_TRUE;
		alphaFlag = ~alphaFlag;
	}

	if (!GLHelper::keystateA)
	{
		alphaFlagTriggered = GL_FALSE;
	}

	// update elapsed time for animation
	if (taskID == 2 || taskID == 7 || taskID == 8)
	{
		animElapsedTime += static_cast<float>(GLHelper::delta_time);
	}
	else // reset elapsed time when not in use
	{
		animElapsedTime = 0.f;
	}

	// implements ease in/out animation and updates tileSize
	if (taskID == 2 )
	{
		float minSize = 16.f, maxSize = 256.f, easeTime;

		easeTime = ((glm::sin(glm::pi<float>() * animElapsedTime / animTime
				   - glm::pi<float>() / 2.f) + 1.f) / 2.f);
		
		tileSize = minSize + easeTime * (maxSize - minSize);

	}
}

/*  _________________________________________________________________________ */
/*! GLApp::draw
 * @brief Draw the GLApp.
 *
 * This function draws the GLApp by performing the following tasks:
 * 1. Writes the window title with various information.
 * 2. Clears the color buffer.
 * 3. Draw rectangle model to viewport.
 *
 * @param none
 * @return void
*/
void GLApp::draw()
{	
	// window title variables
	std::stringstream title;
	std::string taskStr;

	// sets string for the taskID that is active
	switch (taskID)
	{
	case 0:
		taskStr = "Task 0: Paint Color | ";
		break;
	case 1:
		taskStr = "Task 1: Fixed-Size Checkerboard | ";
		break;
	case 2:
		taskStr = "Task 2: Animated Checkerboard | ";
		break;
	case 3:
		taskStr = "Task 3: Texture Mapping | ";
		break;
	case 4:
		taskStr = "Task 4: Repeating | ";
		break;
	case 5:
		taskStr = "Task 5: Mirroring | ";
		break;
	case 6:
		taskStr = "Task 6: Clamping | ";
		break;
	case 7:
		taskStr = "Task 7: Special effects | ";
		break;
	case 8:
		taskStr = "Task 8: Rotating tunnel | ";
		break;
	}

	title << "Tutorial 5 | Brandon Ho Jun Jie | " << taskStr << "Alpha Blend: "
		<< (alphaFlag ? "ON" : "OFF") << " | Modulate: " << (modFlag ? "ON" : "OFF");

	glfwSetWindowTitle(GLHelper::ptr_window, title.str().c_str());

	// clear buffer with color set in GLApp::init()
	glClear(GL_COLOR_BUFFER_BIT);

	// now, render rectangular model from NDC coordinates to viewport
	mdl.draw();
}

/*  _________________________________________________________________________ */
/*! GLApp::cleanup
 * @brief Empty.
 *
 * @param none
 * @return none
*/
void GLApp::cleanup() {
  // empty for now
}

/*  _________________________________________________________________________ */
/*! GLApp::GLModel::setup_vao
 * @brief Set up the vertex array object (VAO) for the model.
 *
 * This method sets up the vertex array object (VAO) for the model. It performs the following tasks:
 * 1. Defines the vertex position, color attributes and texture coordinates.
 * 2. Transfers the vertex data to a vertex buffer object (VBO).
 * 3. Creates and configures the VAO with vertex attribute bindings and formats.
 * 4. Specifies the primitive type and the index buffer for rendering.
 *
 * @param none
 * @return void
*/
void GLApp::GLModel::setup_vao()
{
	// Define vertex position and color attributes
	std::vector<Vertex> vertices{
	Vertex{glm::vec2{1.f, -1.f}, glm::vec3{1.f, 0.f, 0.f}, glm::vec2{1.f, 0.f}},
	Vertex{glm::vec2{1.f,  1.f}, glm::vec3{0.f, 1.f, 0.f}, glm::vec2{1.f, 1.f}},
	Vertex{glm::vec2{-1.f, 1.f}, glm::vec3{0.f, 0.f, 1.f}, glm::vec2{0.f, 1.f}},
	Vertex{glm::vec2{-1.f, -1.f}, glm::vec3{1.f, 0.f, 1.f}, glm::vec2{0.f, 0.f}}
	};

	// transfer vertex position and color attributes to VBO
	GLuint vbo_hdl;
	glCreateBuffers(1, &vbo_hdl);
	glNamedBufferStorage(vbo_hdl, sizeof(Vertex) * vertices.size(),
						 vertices.data(), GL_DYNAMIC_STORAGE_BIT);

	// encapsulate information about contents of VBO and VBO handle
	// to another object called VAO
	glCreateVertexArrays(1, &vaoid);

	// for vertex position data, we use vertex attribute index 0
	// and vertex buffer binding point 4
	glEnableVertexArrayAttrib(vaoid, 0);
	glVertexArrayVertexBuffer(vaoid, 4, vbo_hdl, 0, sizeof(Vertex));
	glVertexArrayAttribFormat(vaoid, 0, 2, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribBinding(vaoid, 0, 4);

	// for vertex color data, we use vertex attribute index 1
	glEnableVertexArrayAttrib(vaoid, 1);
	glVertexArrayAttribFormat(vaoid, 1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec2));
	glVertexArrayAttribBinding(vaoid, 1, 4);

	// for vertex texture data, we use vertex attribute index 2
	glEnableVertexArrayAttrib(vaoid, 2);
	glVertexArrayAttribFormat(vaoid, 2, 2, GL_FLOAT, GL_FALSE, 
							  sizeof(glm::vec2) + sizeof(glm::vec3));
	glVertexArrayAttribBinding(vaoid, 2, 4);

	primitive_type = GL_TRIANGLE_STRIP;
	std::vector<GLushort> idx_vtx{ 0, 1, 2, 2, 3, 0 };
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
/*! GLApp::GLModel::setup_shdrpgm
 * @brief Set up the shader program for the model.
 *
 * This method sets up the shader program for the model. It performs the following tasks:
 * 1. Defines the shader files for the vertex and fragment shaders.
 * 2. Compiles, links, and validates the shader program.
 * 3. Checks if the shader program is successfully linked and exits with an error message if not.
 *
 * @return void
*/
void GLApp::GLModel::setup_shdrpgm() {
	std::vector<std::pair<GLenum, std::string>> shdr_files;

	shdr_files.emplace_back(std::make_pair(
		GL_VERTEX_SHADER,
		"../shaders/my-tutorial-5.vert"));

	shdr_files.emplace_back(std::make_pair(
		GL_FRAGMENT_SHADER,
		"../shaders/my-tutorial-5.frag"));
	shdr_pgm.CompileLinkValidate(shdr_files);

	if (GL_FALSE == shdr_pgm.IsLinked()) {
		std::cout << "Unable to compile/link/validate shader programs" << "\n";
		std::cout << shdr_pgm.GetLog() << std::endl;
		std::exit(EXIT_FAILURE);
	}
}

/*  _________________________________________________________________________ */
/*! GLApp::GLModel::draw
 * @brief Draw the model.
 *
 * This method performs the following tasks to draw the model:
 * 1. Enables or disables alpha blending based on the alphaFlag.
 * 2. Binds texobj1 to texture unit 6, binds texobj2 for taskID 8.
 * 3. Sets the texture sampling mode based on the taskID.
 * 4. Enables the shader program.
 * 5. Passes various uniform variables to the shader program.
 * 6. Specifies the VAO's state to be used for rendering.
 * 7. Calls glDrawElements to perform the rendering.
 * 8. Resets the current VAO and shader program after rendering.
 * 
 * @param none
 * @return void
*/
void GLApp::GLModel::draw()
{
	// turn on alpha blending if alphaFlag is set
	if (alphaFlag)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	else
	{
		glDisable(GL_BLEND);
	}

	// bind texobj1 to texture unit 6
	glBindTextureUnit(6, texobj1);

	// based on the task ID, switch texture sampling mode
	switch (taskID)
	{
	case 3:
	case 4:
		glTextureParameteri(texobj1, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(texobj1, GL_TEXTURE_WRAP_T, GL_REPEAT);
		break;
	case 5:
		glTextureParameteri(texobj1, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		glTextureParameteri(texobj1, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
		break;
	case 6:
		glTextureParameteri(texobj1, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(texobj1, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		break;
	case 8: // bonus task, bind tex obj2 to texture unit 6
		
		glBindTextureUnit(6, texobj2);
		glTextureParameteri(texobj2, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(texobj2, GL_TEXTURE_WRAP_T, GL_REPEAT);
		break;
	}

	// enable shader program
	shdr_pgm.Use();

	// pass taskID to shader uniform variable in shader
	GLint uniform_taskID_loc = glGetUniformLocation(shdr_pgm.GetHandle(), "u_taskID");
	if (uniform_taskID_loc >= 0)
	{
		glUniform1ui(uniform_taskID_loc, taskID);
	}
	else
	{
		std::cout << "Uniform variable u_taskID doesn't exist!!!\n";
		std::exit(EXIT_FAILURE);
	}
	
	// pass modulate flag to shader uniform variable in shader
	GLint uniform_modFlag_loc = glGetUniformLocation(shdr_pgm.GetHandle(), "u_modFlag");
	if (uniform_modFlag_loc >= 0)
	{
		glUniform1i(uniform_modFlag_loc, modFlag);
	}
	else
	{
		std::cout << "Uniform variable u_modFlag doesn't exist!!!\n";
		std::exit(EXIT_FAILURE);
	}

	// pass tile size to uniform variable in shader
	GLint uniform_tileSize_loc = glGetUniformLocation(shdr_pgm.GetHandle(), "u_tileSize");
	if (uniform_tileSize_loc >= 0)
	{
		glUniform1f(uniform_tileSize_loc, tileSize);
	}
	else
	{
		std::cout << "Uniform variable u_tileSize doesn't exist!!!\n";
		std::exit(EXIT_FAILURE);
	}

	// pass resolution to uniform variable in shader
	GLint uniform_resolution_loc = glGetUniformLocation(shdr_pgm.GetHandle(), "u_resolution");
	glm::vec2 resolution{ GLHelper::width, GLHelper::height };
	if (uniform_resolution_loc >= 0)
	{
		glUniform2fv(uniform_resolution_loc, 1, glm::value_ptr(resolution));
	}
	else
	{
		std::cout << "Uniform variable u_resolution doesn't exist!!!\n";
		std::exit(EXIT_FAILURE);
	}

	// pass elapsed time to uniform variable in shader
	GLint uniform_time_loc = glGetUniformLocation(shdr_pgm.GetHandle(), "u_time");
	if (uniform_time_loc >= 0)
	{
		glUniform1f(uniform_time_loc, animElapsedTime);
	}
	else
	{
		std::cout << "Uniform variable u_time doesn't exist!!!\n";
		std::exit(EXIT_FAILURE);
	}

	// tell fragment shader sampler2D u_tex2d to use texture image unit 6
	GLuint tex_loc = glGetUniformLocation(shdr_pgm.GetHandle(), "u_tex2D");
	glUniform1i(tex_loc, 6);
	if (uniform_time_loc >= 0)
	{
		glUniform1f(uniform_time_loc, animElapsedTime);
	}
	else
	{
		std::cout << "Uniform variable u_tex2D doesn't exist!!!\n";
		std::exit(EXIT_FAILURE);
	}

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

/*  _________________________________________________________________________ */
/*! GLApp::setup_texobj
 * @brief Set up a texture object.
 *
 * This method performs the following tasks to set up a texture object:
 * 1. Opens the binary file specified by the given pathname.
 * 2. Reads the image data from the file and stores it in client memory.
 * 3. Creates a texture object handle to encapsulate the texture.
 * 4. Allocates GPU storage for the texture image data.
 * 5. Copies the image data from client memory to the GPU texture buffer memory.
 * 6. Deletes the client memory since the image is now buffered in GPU memory.
 * 7. Returns the handle to the texture object.
 *
 * @param[in] pathname The path to the binary file containing the texture image data.
 * @return The handle to the texture object.
*/
GLuint GLApp::setup_texobj(std::string pathname)
{
	// images have width and height of 256 texels and use 32-bit RGBA texel format
	GLuint width{ 256 }, height{ 256 }, bytes_per_texel{ 4 };

	// use standard C++ library to open binary file
	// copy contents to heap memory pointed to by ptr_texels
	std::ifstream ifs{ pathname, std::ios::binary};

	char* ptr_texels;
	ptr_texels = new char[width * height * bytes_per_texel];
	ifs.read(ptr_texels, static_cast<std::streamsize>(width) * height * bytes_per_texel);
	ifs.close();

	// define and initialize a handle to texture object that will
	// encapsulate two-dimensional textures
	GLuint texobj_hdl;
	glCreateTextures(GL_TEXTURE_2D, 1, &texobj_hdl);

	// allocate GPU storage for texture image data loaded from file
	glTextureStorage2D(texobj_hdl, 1, GL_RGBA8, width, height);

	// copy image data from client memory to GPU texture buffer memory
	glTextureSubImage2D(texobj_hdl, 0, 0, 0, width, height, GL_RGBA,
						GL_UNSIGNED_BYTE, ptr_texels);

	// client memory not required since image is buffered in GPU memory
	delete[] ptr_texels;

	// return handle to texture object
	return texobj_hdl;
}
