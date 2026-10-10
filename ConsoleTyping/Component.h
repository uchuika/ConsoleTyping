// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include <cstdint>

class Component
{
public:
	// コンストラクタ
	// (更新順序が低いほど、コンポーネントは早く更新される)
	Component(class Actor* owner, int updateOrder = 100);
	// デストラクタ
	virtual ~Component();
	// このコンポーネントをデルタタイムで更新する
	virtual void Update(float deltaTime);
	// このコンポーネントの入力を処理する
	virtual void ProcessInput(const uint8_t* keyState) {}
	// ワールド変換が変わったときに呼ばれる
	virtual void OnUpdateWorldTransform() { }

	int GetUpdateOrder() const { return mUpdateOrder; }
protected:
	// 所有しているアクター
	class Actor* mOwner;
	// コンポーネントの更新順序
	int mUpdateOrder;
};
