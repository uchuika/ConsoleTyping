#pragma once

#include "EScene.h"
#include "Parameter.h"

// シーン変更を通知するリスナーインターフェイス
// 変更は即時ではなく、フレームの最後に適用される
class IOnSceneChangedListener
{
public:
	IOnSceneChangedListener() = default;
	virtual ~IOnSceneChangedListener() = default;
	// scene を push する。stackClear が true ならスタックを空にしてから push
	virtual void onSceneChanged(const EScene scene, const Parameter& parameter, const bool stackClear) = 0;
	// popScene が true なら一番上のシーンを pop する
	virtual void onSceneChanged(const bool popScene) = 0;
};
