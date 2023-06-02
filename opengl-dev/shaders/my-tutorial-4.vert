/*!
@file    my-tutorial-4.vert
@author  brandonjunjie.ho@digipen.edu
@date    6/3/2023

@brief
This file contains the code for the vertex shader.

*//*__________________________________________________________________________*/

#version 450 core

layout (location=0) in vec2 aVertexPosition;

uniform mat3 uModel_to_NDC;

void main()
{
	gl_Position = vec4(vec2(uModel_to_NDC * vec3(aVertexPosition, 1.f)), 0.0, 1.0);
}