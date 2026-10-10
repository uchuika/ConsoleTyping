// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

// GLSL 3.3を要求する
#version 330

// 頂点シェーダからのテクスチャ座標入力
in vec2 fragTexCoord;

// カラーバッファへの出力色に対応する
out vec4 outColor;

// テクスチャのサンプリングに使う
uniform sampler2D uTexture;

// ティント色(rgb)と不透明度(a)
uniform vec4 uColor;

void main()
{
	// テクスチャから色をサンプリングする
	// テクスチャは事前乗算でアップロードされるので、rgbもuColor.aでティントする
	vec4 tex = texture(uTexture, fragTexCoord);
	outColor = vec4(tex.rgb * uColor.rgb * uColor.a, tex.a * uColor.a);
}
