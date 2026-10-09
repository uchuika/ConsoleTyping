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
	void ApplySceneRequests();
	std::shared_ptr<AbstractScene> CreateScene(EScene scene, const Parameter& parameter);

	// Map of textures loaded
	std::unordered_map<std::string, class Texture*> mTextures;
	// Map of fonts loaded
	std::unordered_map<std::string, class Font*> mFonts;

	class Texture* mWhiteTexture;
	class Texture* mCircleTexture;

	// All the actors in the game
	std::vector<class Actor*> mActors;
	// Any pending actors
	std::vector<class Actor*> mPendingActors;

	// All the sprite components drawn
	std::vector<class SpriteComponent*> mSprites;

	// シーンのスタック。一番上のシーンだけが動く
	std::stack<std::shared_ptr<AbstractScene>> mSceneStack;
	std::vector<SceneRequest> mSceneRequests;

	// Sprite shader
	class Shader* mSpriteShader;
	// Sprite vertex array
	class VertexArray* mSpriteVerts;

	SDL_Window* mWindow;
	SDL_GLContext mContext;
	Uint32 mTicksCount;
	bool mIsRunning;
	bool mIsFullScreen;
	// Track if we're updating actors right now
	bool mUpdatingActors;

	// 論理解像度 (投影行列とウィンドウの基準サイズ)
	float mScreenWidth;
	float mScreenHeight;
	// exe のあるディレクトリ (末尾は区切り文字)。Shaders/Assets の基準
	std::string mBasePath;
};
