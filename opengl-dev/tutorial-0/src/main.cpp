/*!
@file    main.cpp
@author  pghali@digipen.edu
@date    10/11/2016

This file uses functionality defined in type GLApp to initialize an OpenGL
context and implement a game loop.

*//*__________________________________________________________________________*/

/*                                                                   includes
----------------------------------------------------------------------------- */
// Extension loader library's header must be included before GLFW's header!!!
#include "../include/glapp.h"
#include <iostream>

/*                                                   type declarations
----------------------------------------------------------------------------- */

/*                                                      function declarations
----------------------------------------------------------------------------- */
static void draw();
static void update();
static void init();

/*                                                   objects with file scope
----------------------------------------------------------------------------- */

/*                                                      function definitions
----------------------------------------------------------------------------- */
/*  _________________________________________________________________________ */
/*! main

@param none

@return int

Indicates how the program existed. Normal exit is signaled by a return value of
0. Abnormal termination is signaled by a non-zero return value.
Note that the C++ compiler will insert a return 0 statement if one is missing.
*/
int main() {
  // start with a 16:9 aspect ratio
  if (!GLApp::init(2400, 1350, "Tutorial 0 | Setting up OpenGL 4.5")) {
      std::cout << "Unable to create OpenGL context" << std::endl;
    std::exit(EXIT_FAILURE);
  }

  // window's close flag is set by clicking close widget or Alt+F4
  while (!glfwWindowShouldClose(GLApp::ptr_window)) {
    draw();
    update();
  }
  
  GLApp::cleanup();
}

/*  _________________________________________________________________________ */
/*! draw

@param none

@return none

Call application to draw and then swap front and back frame buffers ...
Uses GLHelper::GLFWWindow* to get handle to OpenGL context.
*/
static void draw() {
  // render scene
  GLApp::draw();

  // swap buffers: front <-> back
  // GLApp::ptr_window is handle to window that defines the OpenGL context
  glfwSwapBuffers(GLApp::ptr_window); 
}

/*  _________________________________________________________________________ */
/*! update

@param none

@return none

Make sure callbacks are invoked when state changes in input devices occur.
Ensure time per frame and FPS are recorded.
Let application update state changes (such as animation).
*/
static void update() {
  // process events if any associated with input devices
  glfwPollEvents();

  // main loop computes fps and other time related stuff once for all apps ...
  GLApp::update_time(1.0);

  // animate scene
  GLApp::update();
}
