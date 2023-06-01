/*!
* @file    glhelper.h
* @author  pghali@digipen.edu
* @co-author brandonjunjie.ho@digipen.edu
* @date    6/26/2023
*
* @brief This file contains the declaration of namespace Helper that encapsulates the
*		 functionality required to create an OpenGL context using GLFW; use GLEW
*		 to load OpenGL extensions; initialize OpenGL state; and finally initialize
*		 the OpenGL application by calling initalization functions associated with
*		 objects participating in the application.
*//*__________________________________________________________________________*/

/*                                                                      guard
----------------------------------------------------------------------------- */
#ifndef GLHELPER_H
#define GLHELPER_H

/*                                                                   includes
----------------------------------------------------------------------------- */
#include <GL/glew.h> // for access to OpenGL API declarations 
#include <GLFW/glfw3.h>
#include <string>

/*  _________________________________________________________________________ */
struct GLHelper
  /*! GLHelper structure to encapsulate initialization stuff ...
  */
{
  static bool init(GLint w, GLint h, std::string t);
  static void cleanup();

  // callbacks ...
  static void error_cb(int error, char const* description);
  static void fbsize_cb(GLFWwindow *ptr_win, int w, int h);
  // I/O callbacks ...
  static void key_cb(GLFWwindow *pwin, int key, int scancode, int action, int mod);
  static void mousebutton_cb(GLFWwindow *pwin, int button, int action, int mod);
  static void mousescroll_cb(GLFWwindow *pwin, double xoffset, double yoffset);
  static void mousepos_cb(GLFWwindow *pwin, double xpos, double ypos);

  static void update_time(double fpsCalcInt = 1.0);

  static GLint width, height;
  static GLdouble fps;
  static GLdouble delta_time; // time taken to complete most recent game loop
  static std::string title;
  static GLFWwindow *ptr_window;

  // this flag is true if button P was toggled from released position to pressed
  static GLboolean keystateH;
  static GLboolean keystateK;
  static GLboolean keystateU;
  static GLboolean keystateV;
  static GLboolean keystateZ;

  // this flag is true if left mouse button is clicked
  static GLboolean leftclickState;

  static void print_specs();
};

#endif /* GLHELPER_H */
