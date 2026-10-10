// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "SpriteComponent.h"
#include "Texture.h"
#include "Shader.h"
#include "Actor.h"
#include "Game.h"

SpriteComponent::SpriteComponent(Actor* owner, int drawOrder)
	:Component(owner)
	,mTexture(nullptr)
	,mDrawOrder(drawOrder)
	,mTexWidth(0)
	,mTexHeight(0)
{
	mOwner->GetGame()->AddSprite(this);
}

SpriteComponent::~SpriteComponent()
{
	mOwner->GetGame()->RemoveSprite(this);
}

void SpriteComponent::Draw(Shader* shader)
{
	if (mTexture)
	{
		// テクスチャの幅/高さで四角形をスケールする
		Matrix4 scaleMat = Matrix4::CreateScale(
			static_cast<float>(mTexWidth),
			static_cast<float>(mTexHeight),
			1.0f);
		
		Matrix4 world = scaleMat * mOwner->GetWorldTransform();
		
		// 全てのスプライトは同じシェーダ/頂点を使うので
		// ゲームはスプライトを描画する前にそれらをアクティブにする
		
		// ワールド変換を設定する
		shader->SetMatrixUniform("uWorldTransform", world);
		// UIの色がスプライトに漏れないよう、常にティントをリセットする
		shader->SetColorUniform("uColor", Vector3(1.0f, 1.0f, 1.0f), 1.0f);
		// 現在のテクスチャを設定する
		mTexture->SetActive();
		// 四角形を描画する
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
	}
}

void SpriteComponent::SetTexture(Texture* texture)
{
	mTexture = texture;
	// 幅/高さを設定する
	mTexWidth = texture->GetWidth();
	mTexHeight = texture->GetHeight();
}
