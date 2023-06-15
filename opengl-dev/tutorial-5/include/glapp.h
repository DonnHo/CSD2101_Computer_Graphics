/*!
* @file    glapp.cpp
* @author  pghali@digipen.edu
* @co-author brandonjunjie.ho@digipen.edu
* @date    6/10/2023
*
* @brief This file contains the declaration of namespace GLApp that encapsulates the
*  		 functionality required to implement an OpenGL application including 
*		 compiling, linking, and validating shader programs
*		 setting up geometry and index buffers, 
*		 configuring VAO to present the buffered geometry and index data to
*		 vertex shaders, configuring textures. The context contains the declarations
*		 offlags and variables that are required for
*        texture mapping, ease-in/out animation, blending modes for transparency
*		 and different texture sampling modes.
*
* 		 Extra features: TaskID goes up to 8, additional 2 tasks.
* 
*						 - Task 7 implements special effects using the fragment
*						 shader and renders out a cardioid shape with movement
*						 and colors.
*
*						 - Task 8 implements rotating tunnel effect by mapping
*						   uv coordinates to a circular space and sampling a
*                          texture based on the modified coordinates.
*						   Note: texture used "../images/water-rgba-256.tex"
*//*__________________________________________________________________________*/

/*                                                                      guard
----------------------------------------------------------------------------- */
#ifndef GLAPP_H
#define GLAPP_H

/*                                                                   includes
----------------------------------------------------------------------------- */
#include <glslshader.h>

struct GLApp {

  // previous existing declarations
  static void init();
  static void update();
  static void draw();
  static void cleanup();

  // encapsulates state required to render a geometrical model
  struct GLModel {
	  GLenum primitive_type; // which OpenGL primitive to be rendered?
	  GLSLShader shdr_pgm; // which shader program?
	  GLuint vaoid; // handle to VAO
	  GLuint idx_elem_cnt; // how many elements of primitive of type
	  // primitive_type are to be rendered
	  // member functions defined in glapp.cpp
	  void setup_vao();
	  void setup_shdrpgm();
	  void draw();
  };

  // encapsulates vertex data in array of struct layout
  struct Vertex {
	  glm::vec2 pos;
	  glm::vec3 col;
	  glm::vec2 tex;
  };

  // data member to represent geometric model to be rendered
  // C++ requires this object to have a definition in glapp.cpp!!!
  static GLModel mdl;

  // task id
  static GLuint taskID;

  // flag variables
  static GLboolean taskFlag;
  static GLboolean modFlag;
  static GLboolean modFlagTriggered;
  static GLboolean alphaFlag;
  static GLboolean alphaFlagTriggered;
  
  // bonus shader variables
  static GLfloat animTime;
  static GLfloat animElapsedTime;

  // tileSize for task 2
  static GLfloat tileSize;

  // texture variables
  static GLuint texobj1;
  static GLuint texobj2;

  // initializes texobj with texture file
  static GLuint setup_texobj(std::string pathname);
};


#endif /* GLAPP_H */
