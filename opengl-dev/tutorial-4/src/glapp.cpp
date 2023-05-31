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

// file scope
std::random_device rd; // random device for seed
std::default_random_engine gen(rd()); // seeded random engine

// get uniformed distribution from [-1,1]
std::uniform_real_distribution<float> urdf(-1.f,
	std::nextafter(1.f, std::numeric_limits<float>::max()));

/*  _________________________________________________________________________ */
/*! randColor
 * @brief Generate a random color.
 *
 * This function generates a random color by generating random values for the
 * red, green, and blue components in the range [0, 1]. The random values are
 * generated using a uniform distribution.
 *
 * @param none
 * @return A random color represented as a glm::vec3 object.
*/
glm::vec3 randColor()
{
	return glm::vec3{ (urdf(gen) + 1.f) / 2.f,
					  (urdf(gen) + 1.f) / 2.f,
					  (urdf(gen) + 1.f) / 2.f };
}

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

	GLApp::VPSS shdr_files_names{
		std::make_pair<std::string, std::string>
		("../shaders/my-tutorial-3.vert", "../shaders/my-tutorial-3.frag")
	};

	// create shader program from shader files in vector shdr_file_names
	// and insert each shader program into the container GLApp::shdrpgms
	GLApp::init_shdrpgms_cont(shdr_files_names);

	// Part 4: create different geometries and insert them into
	// repositor container GLApp::models
	GLApp::init_models_cont();

	// GLApp::objects empty since simulation begins with no objects displayed
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

	glClearColor(1.f, 1.f, 1.f, 1.f);

	// Part 1: Update polygon rasterization mode ...
	// Check if key 'P' is pressed
	// If pressed, update polygon rasterization mode

	static GLuint rasterMode{ 0 };
	if (GLHelper::keystateP == GL_TRUE)
	{
		switch (rasterMode)
		{
		case 0:
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			break;
		case 1:
			glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
			break;
		case 2:
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			break;
		}
		rasterMode = ++rasterMode % 3;
		GLHelper::keystateP = GL_FALSE;
	}

	// Part 2: Spawn or kill objects ...
	// Check if left mouse button is pressed
	// If maximum object limit is not reached, spawn new object(s)
	// Otherwise, kill oldest objects
	size_t maxLimit{ 32678 };
	size_t minLimit{ 1 };
	static GLboolean spawn{ GL_TRUE };

	if (GLHelper::leftclickState == GL_TRUE)
	{
		// if spawn is true
		if (spawn)
		{
			// set spawn to false if maxLimit is reached
			if (objects.size() >= maxLimit)
			{
				spawn = GL_FALSE;
			}
			else if (objects.empty()) // spawns the first object
			{
				GLApp::GLObject obj{};
				obj.init();
				objects.emplace_back(obj);
				++GLObject::objCount[obj.mdl_ref];
			}
			else
			{
				size_t size = objects.size();
				for (size_t i{ 0 }; i < size; ++i)
				{
					GLApp::GLObject obj{};
					obj.init();
					objects.emplace_back(obj);
					++GLObject::objCount[obj.mdl_ref];
				}
			}
		}
		
		// if spawn is false
		if(!spawn)
		{
			// sets spawn to true if minLimit is reached
			if (objects.size() == minLimit)
			{
				spawn = GL_TRUE;
				GLApp::GLObject obj{};
				obj.init();
				objects.emplace_back(obj);
				++GLObject::objCount[obj.mdl_ref];
			}
			else
			{
				size_t size = objects.size();
				for (size_t i{ 0 }; i < size/2; ++i)
				{
					--GLObject::objCount[objects.front().mdl_ref];
					objects.pop_front();
				}
			}
		}

		GLHelper::leftclickState = GL_FALSE;
	}

	// Part 3:
	// for each object in container GLApp::objects
	// Update object's orientation
	// A more elaborate implementation would animate the object's movement
	// A much more elaborate implementation would animate the object's size
	// Using updated attributes, compute world-to-ndc transformation matrix
	for (auto& tmp : objects)
	{
		tmp.update(GLHelper::delta_time);
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

	title << "Tutorial 3 | Brandon Ho Jun Jie | " << "Obj: " << objects.size() << " | "
											      << "Box: " << GLObject::objCount[0] << " | "
												  << "Mystery: " << GLObject::objCount[1] << " | "
												  << std::setprecision(2) << std::fixed << GLHelper::fps;
	
	glfwSetWindowTitle(GLHelper::ptr_window, title.str().c_str());

	// clear back buffer
	glClear(GL_COLOR_BUFFER_BIT);

	// Part 3: Special rendering modes
	// Use status of flag GLHelper::keystateP to set appropriate polygon
	// rasterization mode using glPolygonMode
	// if rendering GL_POINT, control diameter of rasterized points
	// using glPointSize
	// if rendering GL_LINE, control width of rasterized lines
	// using glLineWidth
	GLint polygonMode{};
	glGetIntegerv(GL_POLYGON_MODE, &polygonMode);

	switch (polygonMode)
	{
	case GL_LINE:
		glLineWidth(5.f);
		break;
	case GL_POINT:
		glPointSize(10.f);
		break;
	default:
		glLineWidth(1.f);
		glPointSize(1.f);
		break;
	}

	// Part 4: Render each object in container GLApp::objects
	for (auto const& x : GLApp::objects) {
		x.draw(); // call member function GLObject::draw()
	}
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
		obj.shd_ref = models.find(shdr_pgm_name);

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

			if (!idx_vtx.empty()) // if index array is empty
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
/*! GLApp::box_model()
 * @brief Create a model representing a square.
 *
 * This function creates a model representing a square by generating vertex and
 * index data, creating a VAO to encapsulate the vertex data, and returning an
 * initialized instance of GLApp::GLModel.
 *
 * @param none
 * @return GLApp::GLModel The created square model.
*/
GLApp::GLModel GLApp::box_model()
{
	std::vector<glm::vec2> pos_vtx{
		glm::vec2(0.5f, -0.5f), glm::vec2(0.5f, 0.5f),
		glm::vec2(-0.5f, 0.5f), glm::vec2(-0.5f, -0.5f)
	};

	std::vector<glm::vec3> clr_vtx{
		randColor(),randColor(),
		randColor(),randColor()
	};

	std::vector<GLushort> idx_vtx{ 0,1,2,2,3,0 };

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
	mdl.primitive_type = GL_TRIANGLES;
	mdl.draw_cnt = idx_vtx.size(); // number of vertices
	mdl.primitive_cnt = mdl.draw_cnt / 3; // number of primitives (not used)

	// Step 4: Return an appropriately initialized instance of GLApp::GLModel
	return mdl;
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
	shdrpgms[shd_ref].Use();

	// there are many models, each with their own initialized VAO object
	// here, we're saying which VAO's state should be used to set up pipe
	glBindVertexArray(models[mdl_ref].vaoid);

	// Copy object 3x3 model-to-NDC matrix to vertex shader
	GLint uniform_var_loc1 = glGetUniformLocation(GLApp::shdrpgms[shd_ref].GetHandle(),
												  "uModel_to_NDC");

	if (uniform_var_loc1 >= 0)
	{
		glUniformMatrix3fv(uniform_var_loc1, 1, GL_FALSE,
						   glm::value_ptr(GLApp::GLObject::mdl_to_ndc_xform));
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

	glDrawElements(models[mdl_ref].primitive_type, models[mdl_ref].draw_cnt, GL_UNSIGNED_SHORT, NULL);
	// after completing the rendering, we tell the driver that VAO
	// vaoid and current shader program are no longer current

	glBindVertexArray(0);
	shdrpgms[shd_ref].UnUse();
}

/*  _________________________________________________________________________ */
/*! GLApp::mystery_model()
 * @brief Create a model representing a mystery shape.
 *
 * This function creates a model representing a mystery shape by generating
 * vertex and index data, creating a VAO to encapsulate the vertex data,
 * and returning an initialized instance of GLApp::GLModel.
 *
 * @param none
 * @return GLApp::GLModel The created mystery shape model.
*/
GLApp::GLModel GLApp::mystery_model()
{
	std::vector<glm::vec2> pos_vtx{
		glm::vec2(-0.25f, -0.5f), glm::vec2(0.3f, 0.1f),
		glm::vec2(-0.1f, -0.1f), glm::vec2(0.f, 0.1f),
		glm::vec2(-0.3f, -0.1f), glm::vec2(0.2f,0.5f),
		glm::vec2(-0.1f,0.5f)
	};

	std::vector<glm::vec3> clr_vtx{ 
		randColor(),randColor(),
		randColor(),randColor(),
		randColor(),randColor(),
		randColor()
	};

	std::vector<GLushort> idx_vtx{ 0,1,2,2,1,3,3,5,6,6,4,3,3,4,2 };

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
	mdl.primitive_type = GL_TRIANGLES;
	mdl.draw_cnt = idx_vtx.size(); // number of vertices
	mdl.primitive_cnt = mdl.draw_cnt; // number of primitives (not used)

	// Step 4: Return an appropriately initialized instance of GLApp::GLModel
	return mdl;
}

/*  _________________________________________________________________________ */
/*! GLApp::GLObject::init()
 * @brief Initialize the GLObject.
 *
 * This function initializes the GLObject by assigning random values to its
 * member variables such as mdl_ref, position, scaling, angle_disp, and angle_speed.
 *
 * @param none
 * @return void
*/
void GLApp::GLObject::init()
{
	// get uniformed distribution for 0 and 1
	std::uniform_int_distribution<> intDis(0, 1);
	
	mdl_ref = intDis(gen);
	shd_ref = 0;

	double const worldRange{ 5000.0 };

	// position of the object in game world in the range [-5000,5000]
	position = glm::vec2(urdf(gen) * worldRange,
						 urdf(gen) * worldRange);

	// non-uniform scaling of the object from in the range [50.0,400.0]
	scaling = glm::vec2(((urdf(gen) + 1.f) / 2.f) * (400.f - 50.f) + 50.f,
						((urdf(gen) + 1.f) / 2.f) * (400.f - 50.f) + 50.f);

	// initialize initial angular displacement and angular speed of the object
	angle_disp = urdf(gen) * 360.f;
	angle_speed = urdf(gen) * 30.f;	
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

	angle_disp += angle_speed * static_cast<float>(delta_time);
	float angleRadians{ glm::radians<float>(angle_disp) };

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

	glm::mat3 modelMat{
		1.f/5000.f, 0.f, 0.f,
		0.f, 1.f/5000.f, 0.f,
		0.f, 0.f, 1.f
	};

	// matrix to map geometry from model to world to NDC coordinates
	mdl_to_ndc_xform = modelMat * (transMat * (rotMat * scaleMat));
}
