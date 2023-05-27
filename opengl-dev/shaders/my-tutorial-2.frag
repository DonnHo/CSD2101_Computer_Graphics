/*!
@file    my-tutorial-2.frag
@author  brandonjunjie.ho@digipen.edu
@date    5/18/2023

@brief
This file contains the code for the fragment shader.

*//*__________________________________________________________________________*/

#version 450 core

layout (location=0) in vec3 vInterpColor;
layout (location=0) out vec4 fFragColor;

void main ()
{
	fFragColor = vec4(vInterpColor, 1.0);
}
