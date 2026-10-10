// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Shader.h"
#include "Texture.h"
#include <SDL.h>
#include <fstream>
#include <sstream>

Shader::Shader()
	: mShaderProgram(0)
	, mVertexShader(0)
	, mFragShader(0)
{
	
}

Shader::~Shader()
{

}

bool Shader::Load(const std::string& vertName, const std::string& fragName)
{
	// 頂点シェーダとピクセルシェーダをコンパイルする
	if (!CompileShader(vertName,
		GL_VERTEX_SHADER,
		mVertexShader) ||
		!CompileShader(fragName,
			GL_FRAGMENT_SHADER,
			mFragShader))
	{
		return false;
	}
	
	// ここでシェーダプログラムを作成する
	// 頂点/フラグメントシェーダをまとめてリンクする
	mShaderProgram = glCreateProgram();
	glAttachShader(mShaderProgram, mVertexShader);
	glAttachShader(mShaderProgram, mFragShader);
	glLinkProgram(mShaderProgram);
	
	// プログラムが正常にリンクしたか確認する
	if (!IsValidProgram())
	{
		return false;
	}
	
	return true;
}

void Shader::Unload()
{
	// プログラム/シェーダを削除する
	glDeleteProgram(mShaderProgram);
	glDeleteShader(mVertexShader);
	glDeleteShader(mFragShader);
}

void Shader::SetActive()
{
	// このプログラムをアクティブに設定する
	glUseProgram(mShaderProgram);
}

void Shader::SetMatrixUniform(const char* name, const Matrix4& matrix)
{
	// この名前のuniformを探す
	GLuint loc = glGetUniformLocation(mShaderProgram, name);
	// 行列データをuniformへ送る
	glUniformMatrix4fv(loc, 1, GL_TRUE, matrix.GetAsFloatPtr());
}

void Shader::SetColorUniform(const char* name, const Vector3& rgb, float alpha)
{
	GLint loc = glGetUniformLocation(mShaderProgram, name);
	glUniform4f(loc, rgb.x, rgb.y, rgb.z, alpha);
}

bool Shader::CompileShader(const std::string& fileName,
	GLenum shaderType,
	GLuint& outShader)
{
	// ファイルを開く
	std::ifstream shaderFile(fileName);
	if (shaderFile.is_open())
	{
		// テキスト全体を文字列に読み込む
		std::stringstream sstream;
		sstream << shaderFile.rdbuf();
		std::string contents = sstream.str();
		const char* contentsChar = contents.c_str();
		
		// 指定された種類のシェーダを作成する
		outShader = glCreateShader(shaderType);
		// ソース文字列を設定してコンパイルを試みる
		glShaderSource(outShader, 1, &(contentsChar), nullptr);
		glCompileShader(outShader);
		
		if (!IsCompiled(outShader))
		{
			SDL_Log("Failed to compile shader %s", fileName.c_str());
			return false;
		}
	}
	else
	{
		SDL_Log("Shader file not found: %s", fileName.c_str());
		return false;
	}
	
	return true;
}

bool Shader::IsCompiled(GLuint shader)
{
	GLint status;
	// コンパイル状態を問い合わせる
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	
	if (status != GL_TRUE)
	{
		char buffer[512];
		memset(buffer, 0, 512);
		glGetShaderInfoLog(shader, 511, nullptr, buffer);
		SDL_Log("GLSL Compile Failed:\n%s", buffer);
		return false;
	}
	
	return true;
}

bool Shader::IsValidProgram()
{
	
	GLint status;
	// リンク状態を問い合わせる
	glGetProgramiv(mShaderProgram, GL_LINK_STATUS, &status);
	if (status != GL_TRUE)
	{
		char buffer[512];
		memset(buffer, 0, 512);
		glGetProgramInfoLog(mShaderProgram, 511, nullptr, buffer);
		SDL_Log("GLSL Link Status:\n%s", buffer);
		return false;
	}
	
	return true;
}
