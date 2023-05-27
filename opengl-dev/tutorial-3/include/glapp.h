/*!
* @file    glapp.h
* @author  pghali@digipen.edu
* @co-author brandonjunjie.ho@digipen.edu
* @date    6/26/2023
*
* @brief This file contains the declaration of namespace GLApp that encapsulates the
*		 functionality required to implement an OpenGL application including
*		 compiling, linking, and validating shader programs
*		 setting up geometry and index buffers,
*		 configuring VAO to present the buffered geometry and index data to
*		 vertex shaders,
*		 configuring textures(in later labs),
*		 configuring cameras(in later labs),
*		 and transformations(in later labs).
*//*__________________________________________________________________________*/

/*                                                                      guard
----------------------------------------------------------------------------- */
#ifndef GLAPP_H
#define GLAPP_H

/*                                                                   includes
----------------------------------------------------------------------------- */
#include <glslshader.h>
#include <string>
#include <vector>
#include <list>

struct GLApp {

  // previous existing declarations
  static void init();
  static void update();
  static void draw();
  static void cleanup();

  // encapsulates state required to render a geometrical model
  struct GLModel {
	  GLenum primitive_type; // which OpenGL primitive to be rendered?
	  GLuint primitive_cnt; // added for tutorial 2
	  GLuint vaoid; // handle to VAO

	  GLuint draw_cnt; // added for tutorial 2
  };

  // tutorial 3 - encapsulates state required to update
  // and render an instance of a model
  struct GLObject {
	  // these two variables keep track of the current orientation
	  // of the object.
	  // angle_speed: rate of change of rotation angle per second
	  // angle_disp: current absolute orientation angle
	  // all angles refer to rotation about z-axis
	  GLfloat angle_speed, angle_disp;

	  glm::vec2 scaling; // scaling parameters
	  glm::vec2 position; // translation vector coordinates

	  // compute object's model transform matrix using scaling,
	  // rotation, and translation attributes
	  // computed by CPU and not vertex shader, GPU runs per-vertex which is 
	  // many times more compared to CPU
	  glm::mat3 mdl_to_ndc_xform;

	  // which model is this object an instance of?
	  // models are contained in a vector, need to keep track of
	  // the specific model that was instanced by the index into the vector container
	  GLuint mdl_ref;

	  // how to draw this instanced model?
	  // shader programs are contained in a vector, need to keep track of
	  // specific shader program using index into vector container
	  GLuint shd_ref;

	  // static data member to keep track of object count
	  static std::vector<GLuint> objCount;

	  // member functions defined in glapp.cpp

	  // function to initialize object's state
	  void init();

	  // function to render object's model (specified by index mdl_ref)
	  // uses model transformation matrix mdl_to_ndc_xform matrix
	  // and shader program specified by index shd_ref
	  void draw() const;

	  // function to update the object's model transformation matrix
	  void update(GLdouble delta_time);
  };

  static std::list<GLApp::GLObject> objects; // singleton

  static std::vector<GLSLShader> shdrpgms; // singleton in tutorial 3

  static std::vector<GLModel> models;

  static GLApp::GLModel box_model();

  static GLApp::GLModel mystery_model();

  static void init_models_cont();

  using VPSS = std::vector<std::pair<std::string, std::string>>;
  static void init_shdrpgms_cont(GLApp::VPSS const&);
};
#endif /* GLAPP_H */
