/*!
@file    my-tutorial-5.vert
@author  brandonjunjie.ho@digipen.edu
@date    6/10/2023

@brief
This file contains the code for the vertex shader.

*//*__________________________________________________________________________*/

#version 450 core

layout (location=0) in vec2 aVertexPosition;
layout (location=1) in vec3 aVertexColor;
layout (location=2) in vec2 aTextureCoord;

layout (location=1) out vec3 vColor;
layout (location=2) out vec2 vTexCoord;

// taskID determines the vertex shader code for each task
uniform uint u_taskID;

void main() 
{
	gl_Position = vec4(aVertexPosition, 0.0, 1.0);
	vColor = aVertexColor;
	vTexCoord = aTextureCoord;
	switch(u_taskID)
	{
	case 4:
	case 5:
	case 6:
		// scale texture coordinates by 4
		vTexCoord = aTextureCoord * 4;
		break;
	}
}
