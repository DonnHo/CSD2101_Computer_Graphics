/*!
* @file    glapp.cpp
* @author  pghali@digipen.edu
* @co-author brandonjunjie.ho@digipen.edu
* @date    5/17/2023
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

	  GLSLShader shdr_pgm; // which shader program?


	  // primitive_type are to be rendered
	  // member functions defined in glapp.cpp
	  void setup_shdrpgm(std::string vtx_shdr,
						 std::string frg_shdr);
	  void draw();
  };

  struct GLViewport {
	  GLint x, y;
	  GLsizei width, height;
  };

  static std::vector<GLViewport> vps;

  // data member to represent geometric model to be rendered
  // C++ requires this object to have a definition in glapp.cpp!!!

  static std::vector<GLModel> models;

  static GLApp::GLModel points_model(int slices, int stacks,
									 std::string vtx_shdr,
									 std::string frg_shdr);

  static GLApp::GLModel lines_model(int slices, int stacks,
									std::string vtx_shdr,
									std::string frg_shdr);

  static GLApp::GLModel trifans_model(int slices, 
									  std::string vtx_shdr,
									  std::string frg_shdr);

  static GLApp::GLModel tristrip_model(int slices, int stacks,
									   std::string vtx_shdr,
									   std::string frg_shdr);
};

#endif /* GLAPP_H */
