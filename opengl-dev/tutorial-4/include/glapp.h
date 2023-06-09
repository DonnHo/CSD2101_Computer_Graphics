/*!
* @file    glapp.h
* @author  pghali@digipen.edu
* @co-author brandonjunjie.ho@digipen.edu
* @date    6/3/2023
*
* @brief This file contains the declaration of namespace GLApp that encapsulates the
*		 functionality required to implement an OpenGL application including
*		 compiling, linking, and validating shader programs
*		 setting up geometry and index buffers,
*		 configuring VAO to present the buffered geometry and index data to
*		 vertex shaders,
*		 configuring textures(in later labs),
*		 configuring cameras,
*		 and transformations.
* 
*		 Extra features: Smooth camera follow movement, smooth input system,
*						 changed movement and rotation to time-based and added
*						 a minimap view of the entire area on the bottom right
*						 corner.
*//*__________________________________________________________________________*/

/*                                                                      guard
----------------------------------------------------------------------------- */
#ifndef GLAPP_H
#define GLAPP_H

/*                                                                   includes
----------------------------------------------------------------------------- */
#include <glslshader.h>
#include <glhelper.h>
#include <string>
#include <map>

struct GLApp {

  // previous existing declarations
  static void init();
  static void update();
  static void draw();
  static void cleanup();

  // encapsulates state required to render a geometrical model
  struct GLModel {
	  GLenum primitive_type; // openGL primitive type to render
	  GLuint primitive_cnt; // number of primitives drawn
	  GLuint vaoid; // handle to VAO
	  GLuint draw_cnt; // number of time draw calls made
  };

  // encapsulates state required to update
  // and render an instance of a model
  struct GLObject {
	  

	  glm::vec2 orientation{ 0.f }; 	  // orientation.x is angle_disp
										  // orientation.y is angle_speed
										  // angle_speed: rate of change of rotation angle per second
										  // angle_disp: current absolute orientation angle
										  // specified in degrees

	  glm::vec2 scaling{ 0.f };			  // scaling parameters
	  glm::vec2 position{ 0.f };		  // position relative to world
	  glm::vec3 color{ 0.f }; 	          // color of object	 
	  glm::mat3 mdl_to_ndc_xform{ 0.f };  // model-to-ndc transformation 
	  glm::mat3 mdl_xform{ 0.f };		  // model-to-world transformation
	  glm::mat3 mdl_to_map_xform{ 0.f };  // mini map view transformation

	  // reference to model that object is an instance of
	  std::map<std::string, GLApp::GLModel>::iterator mdl_ref;

	  // reference to shader program used to render the model
	  std::map<std::string, GLSLShader>::iterator shd_ref;

	  // member functions defined in glapp.cpp

	  // function to render object's model (specified by index mdl_ref)
	  // uses model transformation matrix mdl_to_ndc_xform matrix
	  // and shader program specified by index shd_ref
	  void draw(GLboolean draw_map) const;

	  // function to update the object's model transformation matrix
	  void update(GLdouble delta_time);
  };

  struct Camera2D {
	  GLObject* pgo; // pointer to game object with camera
	  glm::vec2 right, up; // camera orientation vectors
	  glm::mat3 view_xform, camwin_to_ndc_xform, world_to_ndc_xform;

	  // additional parameters
	  GLint height{ 1000 };
	  GLfloat ar;

	  // camera follow parameters
	  glm::vec2 cam_pos;
	  GLfloat interpolation;
	  GLboolean cam_follow;

	  // mini map matrices
	  glm::mat3 map_to_ndc_xform, world_map_to_ndc_xform;

	  // window change parameters
	  GLint min_height{ 500 }, max_height{ 2000 };

	  // height is increasing if 1 and decreasing if -1
	  GLint height_chg_dir{ 1 };

	  // increments by which window height is changed per z key press
	  GLint height_chg_val{ 5 };

	  // camera's speed when button U is pressed
	  GLfloat linear_speed{ 2.f };

	  // keyboard button press flags
	  GLboolean camtype_flag{ GL_FALSE };    // button V
	  GLboolean zoom_flag{ GL_FALSE };		 // button Z
	  GLboolean left_turn_flag{ GL_FALSE };	 // button H
	  GLboolean right_turn_flag{ GL_FALSE }; // button K
	  GLboolean move_flag{ GL_FALSE };		 // button U

	  void init(GLFWwindow* pWindow, GLObject* ptr);
	  void update(GLFWwindow*);
  };

  static Camera2D camera2d;

  // stores <object name, object data>
  static std::map<std::string, GLObject> objects; // singleton, stores instanced objects data

  // stores <model name, model data>
  static std::map<std::string, GLApp::GLModel> models; // singleton that stores models data

  // stores <shader program name, shader program>
  static std::map<std::string, GLSLShader> shdrpgms; // singleton that stores shader programs

  static void init_models_cont(std::string);

  // function to insert shader program into container GLApp::shdrpgms
  static void init_shdrpgms(std::string, std::string, std::string);

  // function to parse scene file
  static void init_scene(std::string);
};
#endif /* GLAPP_H */
