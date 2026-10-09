// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

// Request GLSL 3.3
#version 330

// Tex coord input from vertex shader
in vec2 fragTexCoord;

// This corresponds to the output color to the color buffer
out vec4 outColor;

// This is used for the texture sampling
uniform sampler2D uTexture;

// Tint color (rgb) and opacity (a)
uniform vec4 uColor;

void main()
{
	// Sample color from texture
	// Textures are uploaded premultiplied, so tint rgb by uColor.a as well
	vec4 tex = texture(uTexture, fragTexCoord);
	outColor = vec4(tex.rgb * uColor.rgb * uColor.a, tex.a * uColor.a);
}
