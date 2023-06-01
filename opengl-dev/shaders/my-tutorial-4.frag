/*!
@file    my-tutorial-4.frag
@author  brandonjunjie.ho@digipen.edu
@date    5/26/2023

@brief
This file contains the code for the fragment shader.

*//*__________________________________________________________________________*/

#version 450 core

/*
A per-fragment color attribute is no longer received from rasterizer.
Instead per-fragment color attribute is supplied by the application to
a uniform variable: uniform vec3 uColor;
The uniform variable will retain the value for every invocation of the
fragment shader. That is why every fragment of the triangle primitive
rendered by an object has the same color
*/

uniform vec3 uColor;

layout (location=0) out vec4 fFragColor;

void main ()
{
	fFragColor = vec4(uColor, 1.0);
}
