/*!
@file    my-tutorial-5.frag
@author  brandonjunjie.ho@digipen.edu
@date    6/10/2023

@brief
This file contains the code for the fragment shader that computes the output
color of fragments to be displayed on the screen. The fragment shader output
changes depending on the taskID.

*//*__________________________________________________________________________*/

#version 450 core

layout (location=1) in vec3 vInterpColor;
layout (location=2) in vec2 vTexCoord;

layout (location=0) out vec4 fFragColor;

// taskID that determines what technique to calculate the final color
uniform uint u_taskID;

// flag to modulate fFragColor with vInterpColor
uniform bool u_modFlag;

// tileSize for animation
uniform float u_tileSize;

// texture unit to specify texture object
uniform sampler2D u_tex2D;

// elapsed time for animation
uniform float u_time;

// extra feature variables
uniform vec2 u_resolution; // resolution of context

/*  _________________________________________________________________________ */
/*! rotate2D
 * @brief Set up a texture object.
 *
 * This function rotates a input parameter vector with
 * a input parameter angle.
 *
 * @param[in] uv - vector to be rotated
 * @param[in] a - angle to rotate 
 * @return vec2 containing rotated vector.
*/
vec2 rotate2D(vec2 uv, float a)
{
	float s = sin(a);
	float c = cos(a);

	return mat2(c, -s, s, c) * uv;
}

void main ()
{
	switch(u_taskID)
	{
	case 0:

		// assigns interpolated color to final fragment color
		fFragColor = vec4(vInterpColor, 1.0);
		break;
	case 1:  // checkerboard pattern
		{		
		// set color and size of checkboard pattern
		vec3 fColor0 = {1.0, 1.0, 0.0};
		vec3 fColor1 = {0.0, 0.68, 0.94};
		uint fSize = 32;

		// divides the fragcoord with tile size and add x and y components
		// modulo the result and set fragment to color 0 if odd and color 1
		// if even
		if(bool((int(gl_FragCoord.x/fSize) + int(gl_FragCoord.y/fSize))%2))
		{
			fFragColor = vec4(fColor0, 1.0);
		}
		else
		{
			fFragColor = vec4(fColor1, 1.0);
		}
		}
		break;
	case 2: // ease-in/out animation of checkboard pattern
		{
		// set color of checkboard pattern
		vec3 fColor0 = {1.0, 1.0, 0.0};
		vec3 fColor1 = {0.0, 0.68, 0.94};

		// divides the fragcoord with tile size computed in CPU and add x and y 
		// components modulo the result and set fragment to color 0 if odd and 
		// color 1 if even
		if(bool((int(gl_FragCoord.x/u_tileSize) + int(gl_FragCoord.y/u_tileSize))%2))
		{
			fFragColor = vec4(fColor0, 1.0);
		}
		else
		{
			fFragColor = vec4(fColor1, 1.0);
		}
		}
		break;
	case 3: // case 3 - 6 samples a texture image
	case 4:
	case 5:
	case 6:
		fFragColor = texture(u_tex2D, vTexCoord);
		break;
	case 7: // creates special effects by manipulating fragment coords
		{
		// calculate the normalized UV coordinates of the current fragment
		vec2 uv = (gl_FragCoord.xy - 0.5 * u_resolution.xy) / u_resolution.y;
		vec3 col = vec3(0.0);

		// rotate current UV coordinates by 90 degrees
		uv = rotate2D(uv, 3.1415 / 2.0);

		// value used to calculate points in parametric equation
		float r = 0.15;
		// 60 objects to be drawn to construct cardioid shape
		for (float i = 0.0; i < 60.0; i++)
		{
			// calculates a factor based on the sin of a elapsed time
			float factor = (sin(u_time) * 0.5 + 0.5) + 0.3;

			// increment factor i
			i += factor;

			float a = i / 3;

			// parametric equation is used to calculate the circular path to draw the point
			float dx = 2 * r * cos(a) - r * cos(2 * a);
			float dy = 2 * r * sin(a) - r * sin(2 * a);

			// accumulates the color adding 0.005 divided by the distance between
			// the current UV coordinates and the calculated point on the circular path
			// factor is multiplied in to give variance in size
			col += 0.01 * factor / length(uv - vec2(dx + 0.1, dy));
		}

		// multiply color by a factor based on the sin of a vector to create 
		// color variations over time
		col *= sin(vec3(0.2,0.6,0.9) * u_time) * 0.15 + 0.25;

		fFragColor = vec4(col, 1.0);
		}
		break;
	case 8: // creates a rotating tunnel effect
		{
		// calculate the normalized UV coordinates of the current fragment
		vec2 uv = (gl_FragCoord.xy - 0.5 * u_resolution.xy) / u_resolution.y;

		// calculate the angle between the positive x-axis and the vector UV
		float a = atan(uv.y, uv.x);

		// calculate the distance from origin to the cuurrent UV
		float r = length(uv);

		// calculate the sampling coordinates for the texture
		vec2 st = vec2(a / 3.1415, 0.1 / r) + 0.2 * u_time;
		
		// sample the 2D texture at the calculated coordinates and retrieve the
		// rgb color value
		vec3 col = texture(u_tex2D, st).rgb; 

		// scale the color intensity based on the distance from the centre of the UV coordinates
		col *= 3.0 * r;

		fFragColor = vec4(col, 1.0);
		}
		break;
	}

	// if mod flag is set, modulate the 2 colors
	if(u_modFlag && u_taskID != 0)
	{
		fFragColor *= vec4(vInterpColor, 1.0);
	}
}
