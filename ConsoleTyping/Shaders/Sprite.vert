// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

// GLSL 3.3を要求する
#version 330

// ワールド変換とビュー投影のuniform
uniform mat4 uWorldTransform;
uniform mat4 uViewProj;

// 属性0は位置、1はテクスチャ座標
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;

// テクスチャ座標を出力に追加する
out vec2 fragTexCoord;

void main()
{
	// 位置を同次座標に変換する
	vec4 pos = vec4(inPosition, 1.0);
	// 位置をワールド空間へ、続いてクリップ空間へ変換する
	gl_Position = pos * uWorldTransform * uViewProj;

	// 変換
	// テクスチャ座標をフラグメントシェーダへ渡す
	fragTexCoord = inTexCoord;
}
