/*!
* @file    glapp.cpp
* @author  pghali@digipen.edu
* @co-author brandonjunjie.ho@digipen.edu
* @date    6/26/2023
*
* @brief This file implements functionality useful and necessary to build OpenGL
*		 applications including use of external APIs such as GLFW to create a
*		 window and start up an OpenGL context and to extract function pointers
*		 to OpenGL implementations. Also contains functions that generates models
*		 and objects to be displayed in the application.* 
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
#include <random>
#include <glm/gtc/type_ptr.inl> // for glm::value_ptr

/*                                                   objects with file scope
----------------------------------------------------------------------------- */
// defining singleton containers
std::map<std::string, GLApp::GLObject> GLApp::objects{};
std::map<std::string, GLApp::GLModel> GLApp::models{};
std::map<std::string, GLSLShader> GLApp::shdrpgms{};

// static variables
std::vector<GLuint> GLApp::GLObject::objCount(2); // count of box_model and mystery_model
GLApp::Camera2D GLApp::camera2d{};

/*  _________________________________________________________________________ */
/*! GLApp::init
 * @brief Initialize the GLApp.
 *
 * This function initializes the GLApp by performing the following tasks:
 * 1. Clears the color buffer to white using glClearColor.
 * 2. Sets the viewport to use the entire window.
 * 3. Creates shared shader programs from vertex and fragment shader files
 *    and inserts them into the GLApp::shdrpgms container.
 * 4. Creates different geometries and inserts them into the GLApp::models repository container.
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

	// Part 5: Print OpenGL context and GPU specs
}

/*  _________________________________________________________________________ */
/*! GLApp::update
 * @brief Update the GLApp.
 *
 * This function updates the GLApp by performing the following tasks:
 * 1. Clears the color buffer to white using glClearColor.
 * 2. Updates the polygon rasterization mode based on the key 'P' press:
 *    - If 'P' is pressed, the polygon rasterization mode is updated.
 * 3. Spawns or kills objects based on the left mouse button press:
 *    - If the maximum object limit is not reached, new objects are spawned.
 *    - If the maximum object limit is reached, the oldest objects are killed.
 * 4. Updates the orientation and attributes of each object in the GLApp::objects container.
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
 * 1. Writes the window title with information about the tutorial name, object count,
 *    box count, mystery model count, and current FPS.
 * 2. Clears the back buffer using glClear.
 * 3. Adjusts the diameter of rasterized points or width of rasterized lines based
 *  on the rendering mode.
 * 4. Renders each object in the GLApp::objects container by calling the member
 *    function GLObject::draw() for each object.
 *
 * @param none
 * @return void
*/
void GLApp::draw()
{
	// Part 1: write window title with the following (see sample executable):
	// tutorial name - this should be "Tutorial 3"
	// object count - how many objects are being displayed?
	// how many of these objects are boxes?
	// and, how many of these objects are the mystery model?
	// current fps
	// separate each piece of information using " | "
	// see sample executable for example ...
	std::stringstream title;

	title << "Tutorial 4 | Brandon Ho Jun Jie | " << "Obj: " << objects.size() << " | "
											      << "Box: " << GLObject::objCount[0] << " | "
												  << "Mystery: " << GLObject::objCount[1] << " | "
												  << std::setprecision(2) << std::fixed << GLHelper::fps;
	
	glfwSetWindowTitle(GLHelper::ptr_window, title.str().c_str());

	// clear back buffer
	glClear(GL_COLOR_BUFFER_BIT);

	// Part 4: Render each object in container GLApp::objects
	for (auto const& x : objects) {
		if (x.first != "Camera")
		{
			x.second.draw(); // call member function GLObject::draw()
		}
	}

	objects["Camera"].draw();
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
/*! GLApp::init_shdrpgms_cont
 * @brief Initialize shader programs container.
 *
 * This function creates the shader programs from each pair of shader files
 * int vector vpss and inserts them in the container GLApp::shdrpgms.
 *
 * @param vpss[in] A vector of pairs containing the vertex and fragment shader file paths.
 * @return void
*/

// init_shdrpgms **HEADER
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

// init_scene **HEADER
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
 * @brief Initialize the models container.
 *
 * This function initializes the models container in GLApp by adding the box model and the mystery model.
 *
 * @param none
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
	GLApp::GLModel model{};
	std::vector<glm::vec2> pos_vtx;
	std::vector<GLushort> idx_vtx;

	std::string model_name;
	while (std::getline(ifs, line))
	{
		std::istringstream line_iss{ line };
		GLchar prefix;
		
		line_iss >> prefix;

		switch(prefix)
		{
		case 'v':
			GLfloat x, y;
			line_iss >> x >> y;
			pos_vtx.emplace_back(glm::vec2{ x, y });
			break;
		case 't':
			model.primitive_type = GL_TRIANGLES;

			GLushort idx1, idx2, idx3;
			line_iss >> idx1 >> idx2 >> idx3;
			idx_vtx.emplace_back(idx1);
			idx_vtx.emplace_back(idx2);
			idx_vtx.emplace_back(idx3);
			break;
		case 'f':
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
		case 'n':
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
/*! GLApp::GLObject::draw
 * @brief Draw the GLObject.
 *
 * This function renders the GLObject by performing the following tasks:
 * 1. Sets the appropriate shader program for rendering the geometry.
 * 2. Sets up the vertex array object (VAO) for the GLObject.
 * 3. Copies the 3x3 model-to-NDC matrix to the vertex shader.
 * 4. Specifies the primitive type and the number of primitives to be rendered.
 * 5. Cleans up by unbinding the VAO and the shader program.
 *
 * @param none
 * @return void
*/
void GLApp::GLObject::draw() const
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
		glUniformMatrix3fv(uniform_var_mtx_loc, 1, GL_FALSE,
						   glm::value_ptr(this->mdl_to_ndc_xform));
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
/*! GLApp::GLObject::update()
 * @brief Update the GLObject.
 *
 * This function updates the GLObject based on the given delta_time. It calculates
 * the transformation matrix (mdl_to_ndc_xform) of the object using its member 
 * variables such as scaling, position, angle_disp, and angle_speed.
 *
 * @param delta_time The time difference since the last update.
 * @return void
*/
void GLApp::GLObject::update(GLdouble delta_time)
{
	glm::mat3 scaleMat{
		scaling.x, 0.f, 0.f,
		0.f, scaling.y, 0.f,
		0.f, 0.f, 1.f
	};


	orientation.x += orientation.y * static_cast<float>(delta_time);
	float angleRadians{ glm::radians<float>(orientation.x) };

	glm::mat3 rotMat{
		glm::cos(angleRadians), glm::sin(angleRadians), 0.f,
		-glm::sin(angleRadians), glm::cos(angleRadians), 0.f,
		0.f, 0.f, 1.f
	};

	glm::mat3 transMat{
		1.f, 0.f, 0.f,
		0.f, 1.f, 0.f,
		position.x, position.y, 1.f
	};

	mdl_xform = transMat * (rotMat * scaleMat);

	mdl_to_ndc_xform = camera2d.world_to_ndc_xform * mdl_xform;
}

// HEADER***
void GLApp::Camera2D::init(GLFWwindow* pWindow, GLObject* ptr)
{
	// assign address of object with object named "Camera" in std::map
	// container GLApp::objects
	pgo = ptr;

	// compute camera window's aspect ratio
	GLsizei fb_width, fb_height;
	glfwGetFramebufferSize(pWindow, &fb_width, &fb_height);
	ar = static_cast<GLfloat>(fb_width) / fb_height;

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
		-pgo->position.x, -pgo->position.y, 1.f
	};

	// compute other matrices
	camwin_to_ndc_xform = glm::mat3{
		2.f / (ar * height), 0.f, 0.f,
		0.f, 2.f / height, 0.f,
		0.f, 0.f, 1.f
	};

	world_to_ndc_xform = camwin_to_ndc_xform * view_xform;
}

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
		pgo->orientation.x += pgo->orientation.y;
	}
	
	if (right_turn_flag)
	{
		pgo->orientation.x -= pgo->orientation.y;
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
		pgo->position += linear_speed * up;
	}

	// update camera type
	if (camtype_flag)
	{
		view_xform = glm::mat3{
			right.x, up.x, 0.f,
			right.y, up.y, 0.f,
			glm::dot(-right, pgo->position), glm::dot(-up, pgo->position), 1.f
		};
	}
	else
	{
		view_xform = glm::mat3{
			1.f, 0.f, 0.f,
			0.f, 1.f, 0.f,
			-pgo->position.x, -pgo->position.y, 1.f
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