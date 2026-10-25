// ----------------------------------------------------------------
// Based on Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
//
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "SDL.h"
#include <memory>
#include <stack>
#include <unordered_map>
#include <string>
#include <vector>
#include "Math.h"
#include "AbstractScene.h"
#include "IOnSceneChangedListener.h"
#include "Parameter.h"

class Game final : public IOnSceneChangedListener
{
public:
	Game();
	bool Initialize();
	void RunLoop();
	void Shutdown();

	void AddActor(class Actor* actor);
	void RemoveActor(class Actor* actor);

	void AddSprite(class SpriteComponent* sprite);
	void RemoveSprite(class SpriteComponent* sprite);

	class Texture* GetTexture(const std::string& fileName);
	class Font* GetFont(const std::string& fileName);

	// 矩形描画用の 1x1 白テクスチャと、円描画用のテクスチャ
	class Texture* GetWhiteTexture() const { return mWhiteTexture; }
	class Texture* GetCircleTexture() const { return mCircleTexture; }

	float GetScreenWidth() const { return mScreenWidth; }
	float GetScreenHeight() const { return mScreenHeight; }
	// exe のあるディレクトリ (末尾は区切り文字)。problems.txt や scores.json の場所
	const std::string& GetBasePath() const { return mBasePath; }

	// シーン変更は即時ではなく、UpdateGame の最後に適用される
	void onSceneChanged(const EScene scene, const Parameter& parameter, const bool stackClear) override;
	void onSceneChanged(const bool popScene) override;
private:
	// 保留中のシーン変更
	struct SceneRequest
	{
		enum Kind { Push, Pop };
		Kind kind;
		EScene scene;
		Parameter parameter;
		bool stackClear;
	};

	void ProcessInput();
	void UpdateGame();
	void GenerateOutput();
	bool LoadShaders();
	void CreateSpriteVerts();
	bool CreateBuiltinTextures();
	void LoadData();
	void UnloadData();
	void ToggleFullscreen();
	void LoadGameData();
	void ApplySceneRequests();
	std::shared_ptr<AbstractScene> CreateScene(EScene scene, const Parameter& parameter);

	// ロード済みのテクスチャのマップ
	std::unordered_map<std::string, class Texture*> mTextures;
	// ロード済みのフォントのマップ
	std::unordered_map<std::string, class Font*> mFonts;

	class Texture* mWhiteTexture;
	class Texture* mCircleTexture;

	// ゲーム内の全てのアクター
	std::vector<class Actor*> mActors;
	// 保留中のアクター
	std::vector<class Actor*> mPendingActors;

	// 描画される全てのスプライトコンポーネント
	std::vector<class SpriteComponent*> mSprites;

	// シーンのスタック。一番上のシーンだけが動く
	std::stack<std::shared_ptr<AbstractScene>> mSceneStack;
	std::vector<SceneRequest> mSceneRequests;

	// スプライトシェーダ
	class Shader* mSpriteShader;
	// スプライトの頂点配列
	class VertexArray* mSpriteVerts;

	SDL_Window* mWindow;
	SDL_GLContext mContext;
	Uint32 mTicksCount;
	bool mIsRunning;
	bool mIsFullScreen;
	// 現在アクターを更新しているかどうか
	bool mUpdatingActors;

	// 論理解像度 (投影行列とウィンドウの基準サイズ)
	float mScreenWidth;
	float mScreenHeight;
	// 論理解像度に対する実際の描画の拡大率。文字を作る大きさに使う
	float mRenderScale;
	// exe のあるディレクトリ (末尾は区切り文字)。Shaders/Assets の基準
	std::string mBasePath;
};
