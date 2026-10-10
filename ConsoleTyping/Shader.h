// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include <GL/glew.h>
#include <string>
#include "Math.h"

class Shader
{
public:
	Shader();
	~Shader();
	// 指定された名前の頂点/フラグメントシェーダをロードする
	bool Load(const std::string& vertName, const std::string& fragName);
	void Unload();
	// アクティブなシェーダとして設定する
	void SetActive();
	// Matrixのuniformを設定する
	void SetMatrixUniform(const char* name, const Matrix4& matrix);
	// vec4の色uniformを設定する(rgb + alpha)
	void SetColorUniform(const char* name, const Vector3& rgb, float alpha = 1.0f);
private:
	// 指定されたシェーダのコンパイルを試みる
	bool CompileShader(const std::string& fileName,
					   GLenum shaderType,
					   GLuint& outShader);
	
	// シェーダのコンパイルに成功したかの判定
	bool IsCompiled(GLuint shader);
	// 頂点/フラグメントプログラムのリンクを確認
	bool IsValidProgram();
private:
	// シェーダオブジェクトのIDを格納
	GLuint mVertexShader;
	GLuint mFragShader;
	GLuint mShaderProgram;
};
