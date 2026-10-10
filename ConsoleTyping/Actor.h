// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include <vector>
#include "Math.h"
#include <cstdint>

class Actor
{
public:
	enum State
	{
		EActive,
		EPaused,
		EDead
	};

	Actor(class Game* game);
	virtual ~Actor();

	// ゲームから呼び出される更新関数(オーバーライド不可)
	void Update(float deltaTime);
	// アクターにアタッチされたすべてのコンポーネントを更新する(オーバーライド不可)
	void UpdateComponents(float deltaTime);
	// アクター固有の更新コード(オーバーライド可能)
	virtual void UpdateActor(float deltaTime);

	// Gameから呼び出されるProcessInput関数(オーバーライド不可)
	void ProcessInput(const uint8_t* keyState);
	// アクター固有の入力コード(オーバーライド可能)
	virtual void ActorInput(const uint8_t* keyState);

	// ゲッター/セッター
	const Vector2& GetPosition() const { return mPosition; }
	void SetPosition(const Vector2& pos) { mPosition = pos; mRecomputeWorldTransform = true; }
	float GetScale() const { return mScale; }
	void SetScale(float scale) { mScale = scale;  mRecomputeWorldTransform = true; }
	float GetRotation() const { return mRotation; }
	void SetRotation(float rotation) { mRotation = rotation;  mRecomputeWorldTransform = true; }
	
	void ComputeWorldTransform();
	const Matrix4& GetWorldTransform() const { return mWorldTransform; }

	Vector2 GetForward() const { return Vector2(Math::Cos(mRotation), Math::Sin(mRotation)); }

	State GetState() const { return mState; }
	void SetState(State state) { mState = state; }

	class Game* GetGame() { return mGame; }


	// コンポーネントの追加と削除
	void AddComponent(class Component* component);
	void RemoveComponent(class Component* component);
private:
	// アクターの状態
	State mState;

	// トランスフォーム
	Matrix4 mWorldTransform;
	Vector2 mPosition;
	float mScale;
	float mRotation;
	bool mRecomputeWorldTransform;

	std::vector<class Component*> mComponents;
	class Game* mGame;
};
