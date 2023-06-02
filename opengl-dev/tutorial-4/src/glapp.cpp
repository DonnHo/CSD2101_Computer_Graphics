/*!
* @file    glapp.cpp
* @author  pghali@digipen.edu
* @co-author brandonjunjie.ho@digipen.edu
* @date    6/3/2023
*
* @brief This file implements functionality useful and necessary to build OpenGL
*		 applications including use of external APIs such as GLFW to create a
*		 window and start up an OpenGL context and to extract function pointers
*		 to OpenGL implementations. Also contains functions that generates models
*		 and objects to be displayed in the application.
* 
* 		 Extra features: Smooth camera follow movement, smooth input system,
*						 changed movement and rotation to time-based and added
*						 a minimap view of the entire area on the bottom right
*						 corner.
*//*__________________________________________________________________________*/

/*                                                                   includes
----------------------------------------------------------------------------- */
#include <glapp.h>
#include <glhelper.h>
#include <glm/glm.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <glm/gtc/type_ptr.inl> // for glm::value_ptr

/*                                                   objects with file scope
----------------------------------------------------------------------------- */
// defining singleton containers
std::map<std::string, GLApp::GLObject> GLApp::objects;
std::map<std::string, GLApp::GLModel> GLApp::models{};
std::map<std::string, GLSLShader> GLApp::shdrpgms{};

// static variables
GLApp::Camera2D GLApp::camera2d{};

/*  _________________________________________________________________________ */
/*! GLApp::init
 * @brief Initialize the GLApp.
 *
 * This function initializes the GLApp by performing the following tasks:
 * 1. Clears the color buffer to white using glClearColor.
 * 2. Sets the viewport to use the entire window.
 * 3. Parses the scene file and stores models, shader programs and objects in containers.
 * 4. Initializes the 2D camera.
 *
 * @param none
 * @return void
*/
void GLApp::init() 
{
	// Part 1: initialize OpenGL state
	// clear colorbuffer to white
	glClearColor(1.f, 1.f, 1.f, 1.f);

	// Part 2: use the entire window as viewport
	glViewport(0, 0, GLHelper::width, GLHelper::height);

	// Part 3: parse scene file $(SolutionDir)scenes/tutorial-4.scn
	// and store repositories of models of type GLModel in container
	// GLApp::models, store shader programs of type GLSLShader in
	// container GLApp::shdrpgms, and store repositories of objects of
	// type GLObject in container GLApp::objects
	GLApp::init_scene("../scenes/tutorial-4.scn");

	// Part 4: initialize camera
	GLApp::camera2d.init(GLHelper::ptr_window,
						 &GLApp::objects.at("Camera"));
}

/*  _________________________________________________________________________ */
/*! GLApp::update
 * @brief Update the GLApp.
 *
 * This function updates the GLApp by performing the following tasks:
 * 1. Updates the 2D camera using the GLHelper::ptr_window.
 * 2. Iterates through the objects container and calls the update function for
 *    each object, except for the camera object.
 *
 * @param none
 * @return void
*/
void GLApp::update() 
{
		
		// update camera
		GLApp::camera2d.update(GLHelper::ptr_window);

		// iterate through objects container
		// for each object
		for (auto& it : objects)
		{
			// call update except for camera object
			if (it.first != "Camera")
			{
				it.second.update(GLHelper::delta_time);
			}
		}
}

/*  _________________________________________________________________________ */
/*! GLApp::draw
 * @brief Draw the GLApp.
 *
 * This function draws the GLApp by performing the following tasks:
 * 1. Writes the window title with various information.
 * 2. Clears the back buffer.
 * 3. Sets the full viewport size.
 * 4. Renders each object in the GLApp::objects container, except for the camera object.
 * 5. Renders the camera object.
 * 6. Sets the map viewport size using GL_SCISSOR_TEST and glScissor.
 * 7. Clears the color and depth buffers.
 * 8. Renders each object in the GLApp::objects container, except for the camera object, in the map viewport.
 * 9. Renders the camera object in the map viewport.
 * 10. Disables GL_SCISSOR_TEST.
 *
 * @param none
 * @return void
*/
void GLApp::draw()
{
	// write window title
	std::stringstream title;

	title << "Tutorial 4 | Brandon Ho Jun Jie | Camera Position (" << std::setprecision(2)
		  << std::fixed << camera2d.cam_pos.x << ", " << std::setprecision(2)
		  << camera2d.cam_pos.y << ") | Orientation: " << std::setprecision(0) << camera2d.pgo->orientation.x
		  << " degrees | Window height: " << camera2d.height << " | FPS: " << std::setprecision(2) << GLHelper::fps;
	
	glfwSetWindowTitle(GLHelper::ptr_window, title.str().c_str());

	// clear back buffer
	glClear(GL_COLOR_BUFFER_BIT);

	// set full viewport size
	glViewport(0, 0, GLHelper::width, GLHelper::height);

	// Render each object in container GLApp::objects
	for (auto const& obj : objects) {
		if (obj.first != "Camera")
		{
			obj.second.draw(GL_FALSE); // call member function GLObject::draw()
		}
	}

	objects["Camera"].draw(GL_FALSE);

	// set map viewport size
	glEnable(GL_SCISSOR_TEST);

	// restricts rendering to designated area
	glScissor(GLHelper::width - GLHelper::width / 4, 0, GLHelper::width / 4, GLHelper::height / 4);

	// set viewport size to bottom right corner of the window
	glViewport(GLHelper::width - GLHelper::width / 4, 0, GLHelper::width / 4, GLHelper::height / 4);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Render each object again in the minimap area
	for (auto const& obj : objects) {
		if (obj.first != "Camera")
		{
			obj.second.draw(GL_TRUE); // call member function GLObject::draw()
		}
	}

	objects["Camera"].draw(GL_TRUE);
	glDisable(GL_SCISSOR_TEST);
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
/*! GLApp::init_shdrpgms
 * @brief Initialize shader programs.
 *
 * This function initializes shader programs by compiling, linking, and validating
 * vertex and fragment shader files specified by the given names. It then adds the
 * compiled, linked, and validated shader program to the GLApp::shdrpgms container.
 *
 * @param[in] shdr_pgm_name The name of the shader program.
 * @param[in] vtx_shdr_name The name of the vertex shader file.
 * @param[in] frg_shdr_name The name of the fragment shader file.
 * @return void
*/
void GLApp::init_shdrpgms(std::string shdr_pgm_name,
	std::string vtx_shdr_name,
	std::string frg_shdr_name)
{
	std::vector<std::pair<GLenum, std::string>> shdr_files{
		std::make_pair(GL_VERTEX_SHADER, vtx_shdr_name),
		std::make_pair(GL_FRAGMENT_SHADER, frg_shdr_name)
	};

	GLSLShader shdr_pgm;
	shdr_pgm.CompileLinkValidate(shdr_files);
	if (GL_FALSE == shdr_pgm.IsLinked())
	{
		std::cout << "Unable to compile/link/validate shader programs\n";
		std::cout << shdr_pgm.GetLog() << "\n";
		std::exit(EXIT_FAILURE);
	}

	// add compiled, linked and validated shader program to
	// std::map container GLApp::shdrpgms
	GLApp::shdrpgms[shdr_pgm_name] = shdr_pgm;
}

/*  _________________________________________________________________________ */
/*! GLApp::init_scene
*@brief Initialize the scene from a scene file.
*
* This function initializes the scene by reading and parsing a scene file.It reads
* each object's parameters from the file and performs the following tasks:
* 1. Instantiates a GLObject.
* 2. Reads object parameters.
* 3. If model name is not in the GLApp::models container, it adds the model by
*    calling GLApp::init_models_cont().
* 4. If shader program name is not in the GLApp::shdrpgms container, it adds the
*    shader program by calling GLApp::init_shdrpgms().
* 5. Sets the object's model reference (mdl_ref) to point to the corresponding
*    model in the GLApp::models container.
* 6. Sets the object's shader reference (shd_ref) to point to the corresponding
*    shader program in the GLApp::shdrpgms container.
* 7. Inserts the instantiated object into the GLApp::objects container.
*
* @param[in] scene_filename The name of the scene file.
* @return void
*/
void GLApp::init_scene(std::string scene_filename)
{
	std::ifstream ifs{ scene_filename, std::ios::in };

	if (!ifs)
	{
		std::cout << "ERROR: Unable to open scene file: "
				  << scene_filename << "\n";
		exit(EXIT_FAILURE);
	}

	ifs.seekg(0, std::ios::beg);

	std::string line;
	std::getline(ifs, line); // first line is count of objects in scene
	std::istringstream line_sstm{ line };
	int obj_cnt;
	line_sstm >> obj_cnt; // read count of objects in scene
	while (obj_cnt--) // read each object's parameters
	{
		GLObject obj{};

		std::getline(ifs, line); // 1st parameter - model name
		std::istringstream line_modelname{ line };
		std::string model_name;
		line_modelname >> model_name;

		std::getline(ifs, line); // 2nd parameter - object name
		std::istringstream line_objectname{ line };
		std::string object_name;
		line_objectname >> object_name;

		std::getline(ifs, line); // 3rd parameter - shader program details
		std::istringstream line_shdr_pgm{ line };
		std::string shdr_pgm_name, vtx_shdr_filename, frg_shdr_filename;
		line_shdr_pgm >> shdr_pgm_name >> 
		vtx_shdr_filename >> frg_shdr_filename;

		std::getline(ifs, line); // 4th parameter - object rgb parameters
		std::istringstream  line_rgb{ line };
		line_rgb >> obj.color.r >> obj.color.g >> obj.color.b;
		
		std::getline(ifs, line);  // 5th parameter - object scaling factors
		std::istringstream line_scale{ line };
		line_scale >> obj.scaling.x >> obj.scaling.y;

		std::getline(ifs, line); // 6th parameter - object orientation factors
		std::istringstream line_ort{ line };
		line_ort >> obj.orientation.x >> obj.orientation.y;

		std::getline(ifs, line); // 7th parameter - object position in world
		std::istringstream line_pos{ line };
		line_pos >> obj.position.x >> obj.position.y;

		// if model_name is not in models container, add it
		if (!GLApp::models.contains(model_name))
		{
			GLApp::init_models_cont("../meshes/" + model_name + ".msh");
		}		

		// if shdr_pgm_name is not in shdrpgms container, add it
		if (!GLApp::shdrpgms.contains(shdr_pgm_name))
		{
			GLApp::init_shdrpgms(shdr_pgm_name, vtx_shdr_filename, frg_shdr_filename);
		}

		// set mdl_ref to point to model 
		obj.mdl_ref = models.find(model_name);

		// set shd_ref to point to shader program
		obj.shd_ref = shdrpgms.find(shdr_pgm_name);

		// insert instantiated object into objects container
		objects[object_name] = obj;		
	}
}

/*  _________________________________________________________________________ */
/*! GLApp::init_models_cont()
 * @brief Initialize the models container from a model file.
 *
 * This function initializes the models container by reading and parsing a model file.
 * It reads each vertex and index data from the file and performs the following tasks:
 * 1. Reads the vertex and index data from the file.
 * 2. Creates and stores the vertex buffer object (VBO) and vertex array object (VAO) for the model.
 * 3. Generates a VAO handle to encapsulate the VBO(s) and state of this triangle mesh.
 * 4. Defines the VAO handle, enables vertex array attribute 0, and sets up vertex attribute format and binding.
 * 5. Creates and stores the element buffer object (EBO) for the model.
 * 6. Binds the VAO and EBO to the vertex array.
 * 7. Sets the VAO ID, draw count, and primitive count for the model.
 * 8. Inserts the model into the GLApp::models container with the key model_name.
 *
 * @param[in] model_filename The name of the model file.
 * @return void
*/
void GLApp::init_models_cont(std::string model_filename)
{
	std::ifstream ifs{model_filename, std::ios::in };

	if (!ifs)
	{
		std::cout << "ERROR: Unable to open scene file: "
			<< model_filename << "\n";
		exit(EXIT_FAILURE);
	}

	ifs.seekg(0, std::ios::beg);

	std::string line;
	std::string model_name;
	GLApp::GLModel model{};
	std::vector<glm::vec2> pos_vtx;
	std::vector<GLushort> idx_vtx;

	// read each model parameters
	while (std::getline(ifs, line))
	{
		std::istringstream line_iss{ line };
		GLchar prefix;
		
		line_iss >> prefix;
		
		// checks for type of data to be read
		switch(prefix)
		{
		case 'v': // vertex data
			GLfloat x, y;
			line_iss >> x >> y;
			pos_vtx.emplace_back(glm::vec2{ x, y });
			break;
		case 't': // triangle indices
			model.primitive_type = GL_TRIANGLES;

			{
				GLushort idx1, idx2, idx3;
				line_iss >> idx1 >> idx2 >> idx3;
				idx_vtx.emplace_back(idx1);
				idx_vtx.emplace_back(idx2);
				idx_vtx.emplace_back(idx3);
			}
			break;
		case 'f': // triangle fan indices
			model.primitive_type = GL_TRIANGLE_FAN;

			if (idx_vtx.empty()) // if index array is empty
			{
				GLushort idx1, idx2, idx3;
				line_iss >> idx1 >> idx2 >> idx3;
				idx_vtx.emplace_back(idx1);
				idx_vtx.emplace_back(idx2);
				idx_vtx.emplace_back(idx3);
			}
			else
			{
				GLushort idx;
				line_iss >> idx;
				idx_vtx.emplace_back(idx);
			}
			break;
		case 'n': // name of model
			line_iss >> model_name;
			break;
		}			
	}

	// Step 3: Generate a VAO handle to encapsulate the VBO(s) and
	// state of this triangle mesh
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

	GLuint ebo_hdl;
	glCreateBuffers(1, &ebo_hdl);
	glNamedBufferStorage(ebo_hdl, sizeof(GLushort) * idx_vtx.size(),
		reinterpret_cast<GLvoid*>(idx_vtx.data()),
		GL_DYNAMIC_STORAGE_BIT);
	glVertexArrayElementBuffer(vaoid, ebo_hdl);
	glBindVertexArray(0);

	model.vaoid = vaoid; // set up VAO same as in GLApp::points_model
	model.draw_cnt = idx_vtx.size(); // number of vertices
	model.primitive_cnt = model.draw_cnt / 3; // number of primitives (not used)

	models[model_name] = model; // insert model into map with key model_name
}

/*  _________________________________________________________________________ */
/*! GLApp::GLObject::update()
 * @brief Update the object's transformation matrix based on the delta time.
 *
 * This method updates the object's transformation matrix based on the delta time.
 * It performs the following tasks:
 * 1. Constructs a scale matrix using the object's scaling factors.
 * 2. Updates the object's orientation based on the delta time.
 * 3. Constructs a rotation matrix using the updated orientation.
 * 4. Constructs a translation matrix using the object's position.
 * 5. Computes the model transformation matrix by multiplying the scale, rotation,
 *    and translation matrices.
 * 6. Computes the model-to-NDC transformation matrix by multiplying the world-to-NDC
 *    transformation matrix of the 2D camera and the model transformation matrix.
 * 7. Computes the model-to-map transformation matrix by multiplying the world-to-NDC
 *    map transformation matrix of the 2D camera and the model transformation matrix.
 *
 * @param[in] delta_time The time difference between the current frame and the previous frame.
 * @return void
*/
void GLApp::GLObject::update(GLdouble delta_time)
{
	// compute scale matrix
	glm::mat3 scaleMat{
		scaling.x, 0.f, 0.f,
		0.f, scaling.y, 0.f,
		0.f, 0.f, 1.f
	};

	// updates all objects orientation except for the Camera object
	if (&objects["Camera"] != this)
	{
		orientation.x += orientation.y * static_cast<float>(delta_time);
	}

	float angleRadians{ glm::radians<float>(orientation.x) };

	// compute rotation matrix
	glm::mat3 rotMat{
		glm::cos(angleRadians), glm::sin(angleRadians), 0.f,
		-glm::sin(angleRadians), glm::cos(angleRadians), 0.f,
		0.f, 0.f, 1.f
	};
	// compute translation matrix
	glm::mat3 transMat{
		1.f, 0.f, 0.f,
		0.f, 1.f, 0.f,
		position.x, position.y, 1.f
	};

	// compute model to world matrix
	mdl_xform = transMat * (rotMat * scaleMat);

	// compute model to ndc matrix
	mdl_to_ndc_xform = camera2d.world_to_ndc_xform * mdl_xform;

	// compute model to ndc using map view
	mdl_to_map_xform = camera2d.world_map_to_ndc_xform * mdl_xform;
}

/*  _________________________________________________________________________ */
/*! GLApp::GLObject::draw
 * @brief Draw the object using the specified shader program and VAO state.
 *
 * This method is used to render the object using the specified shader program
 * and vertex array object (VAO) state. It performs the following tasks:
 * 1. Sets the specified shader program as the current shader program for rendering.
 * 2. Sets the specified VAO's state as the current VAO state for rendering.
 * 3. Copies the object's color to the fragment shader.
 * 4. Copies the object's model-to-NDC (Normalized Device Coordinates) matrix to the vertex shader.
 * 5. Calls the OpenGL `glDrawElements()` function to render the object.
 * 6. Resets the current VAO and shader program states.
 *
 * @param[in] draw_map A boolean value indicating whether to draw the object with
 * the model-to-map transformation matrix or the model-to-NDC transformation matrix.
 * @return void
*/
void GLApp::GLObject::draw(GLboolean draw_map) const
{
	// there are many shader programs initialized - here we're saying
	// which specific shader program should be used to render geometry
	shd_ref->second.Use();

	// there are many models, each with their own initialized VAO object
	// here, we're saying which VAO's state should be used to set up pipe
	glBindVertexArray(mdl_ref->second.vaoid);

	// copy object color to fragment shader
	GLint uniform_var_col_loc = glGetUniformLocation(shd_ref->second.GetHandle(),
												 "uColor");

	if (uniform_var_col_loc >= 0)
	{
		glUniform3fv(uniform_var_col_loc, 1, glm::value_ptr(color));
	}
	else
	{
		std::cout << "Uniform variable doesn't exist!!!\n";
		std::exit(EXIT_FAILURE);
	}

	// Copy object 3x3 model-to-NDC matrix to vertex shader
	GLint uniform_var_mtx_loc = glGetUniformLocation(shd_ref->second.GetHandle(),
												  "uModel_to_NDC");
	if (uniform_var_mtx_loc >= 0)
	{
		// draws object with matrix depending on the viewport to render to
		if (draw_map)
		{
			glUniformMatrix3fv(uniform_var_mtx_loc, 1, GL_FALSE,
				glm::value_ptr(this->mdl_to_map_xform));
		}
		else
		{
			glUniformMatrix3fv(uniform_var_mtx_loc, 1, GL_FALSE,
				glm::value_ptr(this->mdl_to_ndc_xform));
		}
	}
	else
	{
		std::cout << "Uniform variable doesn't exist!!!\n";
		std::exit(EXIT_FAILURE);
	}

	// here, we're saying what primitive is to be rendered and how many
	// such primitives exist.
	// the graphics driver knows where to get the indices because the VAO
	// containing this state information has been made current ...
	glDrawElements(mdl_ref->second.primitive_type, mdl_ref->second.draw_cnt, GL_UNSIGNED_SHORT, NULL);

	// after completing the rendering, we tell the driver that VAO
	// vaoid and current shader program are no longer current
	glBindVertexArray(0);
	shd_ref->second.UnUse();
}

/*  _________________________________________________________________________ */
/*! GLApp::Camera2D::init
 * @brief Initialize the 2D camera with the provided parameters.
 *
 * This method initializes the 2D camera with the provided parameters. It performs the following tasks:
 * 1. Assigns the address of *ptr to pgo.
 * 2. Computes the aspect ratio of the camera window based on the framebuffer size.
 * 3. Computes the camera's up and right vectors based on the object's orientation.
 * 4. Initializes the camera's view transformation matrix to a free camera position.
 * 5. Computes the camera window to NDC (Normalized Device Coordinates) transformation matrix.
 * 6. Computes the world to NDC transformation matrix by multiplying the camera window
 *    to NDC transformation matrix and the view transformation matrix.
 * 7. Computes the mini map to NDC transformation matrix.
 * 8. Computes the world to NDC map transformation matrix by multiplying the mini map
 *    to NDC transformation matrix and the view transformation matrix.
 *
 * @param[in] pWindow A pointer to the GLFW window.
 * @param[in] ptr A pointer to the GLObject representing the camera.
 * @return void
*/
void GLApp::Camera2D::init(GLFWwindow* pWindow, GLObject* ptr)
{
	// assign address of object with object named "Camera" in std::map
	// container GLApp::objects
	pgo = ptr;

	// compute camera window's aspect ratio
	GLsizei fb_width, fb_height;
	glfwGetFramebufferSize(pWindow, &fb_width, &fb_height);
	ar = static_cast<GLfloat>(fb_width) / fb_height;

	// compute camera trap parameters
	cam_pos = pgo->position;

	float angleRadians{ glm::radians<float>(pgo->orientation.x) };
	// compute camera up and right vectors
	right = glm::vec2{ glm::cos(angleRadians),
					   glm::sin(angleRadians)};

	up = glm::vec2{ -glm::sin(angleRadians),
					 glm::cos(angleRadians) };

	// at startup, camera must be initialized to free camera
	view_xform = glm::mat3{
		1.f, 0.f, 0.f,
		0.f, 1.f, 0.f,
		-cam_pos.x, -cam_pos.y, 1.f
	};

	// compute other matrices
	camwin_to_ndc_xform = glm::mat3{
		2.f / (ar * height), 0.f, 0.f,
		0.f, 2.f / height, 0.f,
		0.f, 0.f, 1.f
	};

	// compute world to ndc matrix
	world_to_ndc_xform = camwin_to_ndc_xform * view_xform;

	// compute mini map matrices
	map_to_ndc_xform = glm::mat3{
		2.f / (ar * max_height), 0.f, 0.f,
		0.f, 2.f / max_height, 0.f,
		0.f, 0.f, 1.f
	};

	world_map_to_ndc_xform = map_to_ndc_xform * view_xform;
}

/*  _________________________________________________________________________ */
/*! GLApp::Camera2D::update
 * @brief Update the 2D camera based on the current state and window parameters.
 *
 * This method updates the 2D camera based on the current state and window parameters.
 * It performs the following tasks:
 * 1. Checks the keyboard button presses to enable camera interactivity.
 * 2. Updates the camera's aspect ratio based on the current framebuffer size.
 * 3. Updates the camera's orientation if the left or right turn flags are set.
 * 4. Updates the camera's up and right vectors if the left or right turn flags are set.
 * 5. Updates the camera's position if the move flag is set.
 * 6. Updates the camera's position for the follow camera effect.
 * 7. Updates the camera's view transformation matrix based on the camera type
 * 8. Implements the camera's zoom effect if the zoom flag is set.
 * 9. Updates the camera's world-to-camera view transformation matrix, window-to-NDC
 *    transformation matrix, and world-to-NDC transformation matrix.
 * 10. Calls the `update()` method of the associated GLObject to update its transformation.
 *
 * @param[in] pWindow A pointer to the GLFW window.
 * @return void
*/
void GLApp::Camera2D::update(GLFWwindow* pWindow)
{
	// check keyboard button presses to enable camera interactivity
	(GLHelper::keystateV == GL_TRUE) ? camtype_flag = GL_TRUE : camtype_flag = GL_FALSE;
	(GLHelper::keystateZ == GL_TRUE) ? zoom_flag = GL_TRUE : zoom_flag = GL_FALSE;
	(GLHelper::keystateH == GL_TRUE) ? left_turn_flag = GL_TRUE : left_turn_flag = GL_FALSE;
	(GLHelper::keystateK == GL_TRUE) ? right_turn_flag = GL_TRUE : right_turn_flag = GL_FALSE;
	(GLHelper::keystateU == GL_TRUE) ? move_flag = GL_TRUE : move_flag = GL_FALSE;

	// update camera aspect ratio - this must be done every frame	
	// because it is possible for the user to change viewport
	// dimensions
	GLsizei fb_width, fb_height;
	glfwGetFramebufferSize(pWindow, &fb_width, &fb_height);
	ar = static_cast<GLfloat>(fb_width) / fb_height;
	
	// update camera's orientation (if required)
	if (left_turn_flag)
	{
		pgo->orientation.x += pgo->orientation.y * static_cast<float>(GLHelper::delta_time) * 150.f;
		pgo->orientation.x = pgo->orientation.x >= 360.f ? 0.f : pgo->orientation.x;
	}
	
	if (right_turn_flag)
	{
		pgo->orientation.x -= pgo->orientation.y * static_cast<float>(GLHelper::delta_time) * 150.f;
		pgo->orientation.x = pgo->orientation.x <= -360.f ? 0.f : pgo->orientation.x;
	}

	// update camera's up and right vectors (if required)
	if (left_turn_flag || right_turn_flag)
	{
		float angleRadians{ glm::radians<float>(pgo->orientation.x) };
		right = glm::vec2{ glm::cos(angleRadians),
					   glm::sin(angleRadians) };

		up = glm::vec2{ -glm::sin(angleRadians),
						 glm::cos(angleRadians) };
	}

	// update camera's position (if required)
	if (move_flag)
	{
		pgo->position += linear_speed * up * static_cast<float>(GLHelper::delta_time) * 150.f;
	}

	// interpolates camera position to camera object position
	interpolation = static_cast<float>(GLHelper::delta_time);
	cam_pos = (1 - interpolation) * cam_pos + interpolation * pgo->position ;

	// update camera type
	if (camtype_flag) // first-person
	{
		view_xform = glm::mat3{
			right.x, up.x, 0.f,
			right.y, up.y, 0.f,
			glm::dot(-right, pgo->position), glm::dot(-up, pgo->position), 1.f
		};
	}
	else // third-person with cam follow
	{
		view_xform = glm::mat3{
			1.f, 0.f, 0.f,
			0.f, 1.f, 0.f,
			-cam_pos.x, -cam_pos.y, 1.f
		};
	}

	// implement camera's zoom effect (if required)
	if (zoom_flag)
	{
		height_chg_dir = (height <= min_height ? 1 : (height >= max_height ? -1 : height_chg_dir));
		height += height_chg_val * height_chg_dir;
	}

	// compute appropriate world-to-camera view transformation matrix
	// compute window-to-NDC transformation matrix
	// compute world-to-NDC transformation matrix
	pgo->update(GLHelper::delta_time);

	camwin_to_ndc_xform = glm::mat3{
		2.f / (ar * height), 0.f, 0.f,
		0.f, 2.f / height, 0.f,
		0.f, 0.f, 1.f
	};

	world_to_ndc_xform = camwin_to_ndc_xform * view_xform;
}
