// ----------------------------------------------------------------
// Based on Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
//
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Game.h"
#include <GL/glew.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <cmath>
#include "Texture.h"
#include "VertexArray.h"
#include "Shader.h"
#include <algorithm>
#include "Actor.h"
#include "SpriteComponent.h"
#include "Font.h"
#include "Keyboard.h"
#include "ProblemSet.h"
#include "ScoreBoard.h"
#include "WordFilter.h"
#include "WindowIme.h"
#include "MainMenuScene.h"
#include "RankingScene.h"
#include "TypingScene.h"

Game::Game()
:mWhiteTexture(nullptr)
,mCircleTexture(nullptr)
,mSpriteShader(nullptr)
,mSpriteVerts(nullptr)
,mWindow(nullptr)
,mIsRunning(true)
,mIsFullScreen(false)
,mUpdatingActors(false)
,mScreenWidth(1280.0f)
,mScreenHeight(720.0f)
,mRenderScale(1.0f)
{

}

bool Game::Initialize()
{
	// OS の表示倍率による引き伸ばしを避け、実際の画面ピクセルで描く
	SDL_SetHint(SDL_HINT_WINDOWS_DPI_AWARENESS, "permonitorv2");

	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		return false;
	}

	char* basePath = SDL_GetBasePath();
	if (basePath)
	{
		mBasePath = basePath;
		SDL_free(basePath);
	}

	// OpenGLの属性を設定する
	// コアOpenGLプロファイルを使う
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	// バージョン3.3を指定する
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	// RGBA各チャンネル8bitのカラーバッファを要求する
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	// ダブルバッファを有効にする
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	// OpenGLにハードウェアアクセラレーションを使わせる
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

	mWindow = SDL_CreateWindow("ConsoleTyping", 100, 100,
							   static_cast<int>(mScreenWidth),
							   static_cast<int>(mScreenHeight),
							   SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	if (!mWindow)
	{
		SDL_Log("Failed to create window: %s", SDL_GetError());
		return false;
	}

	// OpenGLコンテキストを作成する
	mContext = SDL_GL_CreateContext(mWindow);

	// GLEWを初期化する
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		SDL_Log("Failed to initialize GLEW.");
		return false;
	}

	// 一部のプラットフォームではGLEWが無害なエラーコードを出す
	// それをクリアする
	glGetError();

	if (IMG_Init(IMG_INIT_PNG) == 0)
	{
		SDL_Log("Unable to initialize SDL_image: %s", IMG_GetError());
		return false;
	}

	if (TTF_Init() != 0)
	{
		SDL_Log("Unable to initialize SDL_ttf: %s", TTF_GetError());
		return false;
	}
	
	// シェーダを作成/コンパイルできることを確認する
	if (!LoadShaders())
	{
		SDL_Log("Failed to load shaders.");
		return false;
	}

	// スプライト描画用の四角形を作成する
	CreateSpriteVerts();

	DisableIme(mWindow);
	LoadGameData();

	if (!CreateBuiltinTextures())
	{
		SDL_Log("Failed to create builtin textures.");
		return false;
	}

	LoadData();

	mTicksCount = SDL_GetTicks();

	return true;
}

void Game::RunLoop()
{
	while (mIsRunning)
	{
		ProcessInput();
		UpdateGame();
		GenerateOutput();
	}
}

void Game::LoadGameData()
{
	std::vector<std::string> warnings;
	ProblemSet::getIns()->Load(mBasePath + "problems.txt", &warnings);
	if (!WordFilter::getIns()->Load(mBasePath + "ng-words.txt"))
	{
		warnings.push_back("ng-words.txt not found; no words are blocked");
	}
	std::string scoreMessage;
	ScoreBoard::getIns()->Load(mBasePath + "scores.json", &scoreMessage);
	if (!scoreMessage.empty())
	{
		warnings.push_back(scoreMessage);
	}

	for (const std::string& warning : warnings)
	{
		SDL_Log("%s", warning.c_str());
	}
}

void Game::ProcessInput()
{
	Keyboard::getIns()->BeginFrame();

	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			case SDL_QUIT:
				mIsRunning = false;
				break;
			case SDL_KEYDOWN:
				// キーリピートは無視して、押した瞬間だけをシーンへ渡す
				if (event.key.repeat == 0)
				{
					Keyboard::getIns()->OnKeyDown(event.key.keysym.scancode);
				}
				// Alt+Enter でフルスクリーンとウィンドウを切り替える
				if (event.key.repeat == 0
					&& event.key.keysym.sym == SDLK_RETURN
					&& (event.key.keysym.mod & KMOD_ALT))
				{
					ToggleFullscreen();
				}
				break;
			case SDL_WINDOWEVENT:
				// フォーカスを失うと Alt の KEYUP が届かないことがあるのでリセット
				if (event.window.event == SDL_WINDOWEVENT_FOCUS_LOST)
				{
					SDL_ResetKeyboard();
				}
				break;
		}
	}

	Keyboard::getIns()->Update();

	const Uint8* keyState = SDL_GetKeyboardState(NULL);

	mUpdatingActors = true;
	for (auto actor : mActors)
	{
		actor->ProcessInput(keyState);
	}
	mUpdatingActors = false;
}

void Game::ToggleFullscreen()
{
	mIsFullScreen = !mIsFullScreen;
	SDL_SetWindowFullscreen(mWindow, mIsFullScreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
}

void Game::UpdateGame()
{
	// デルタタイムを計算する
	// 最後のフレームから16ms経過するまで待つ
	while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16))
		;

	float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;
	if (deltaTime > 0.05f)
	{
		deltaTime = 0.05f;
	}
	mTicksCount = SDL_GetTicks();

	// 全てのアクターを更新
	mUpdatingActors = true;
	for (auto actor : mActors)
	{
		actor->Update(deltaTime);
	}
	mUpdatingActors = false;

	// 一番上のシーンだけを更新する
	if (!mSceneStack.empty())
	{
		mSceneStack.top()->update(deltaTime);
	}

	// 保留中のアクターをmActorsに移動する
	for (auto pending : mPendingActors)
	{
		pending->ComputeWorldTransform();
		mActors.emplace_back(pending);
	}
	mPendingActors.clear();

	// 死んだアクターを一時ベクターに追加する
	std::vector<Actor*> deadActors;
	for (auto actor : mActors)
	{
		if (actor->GetState() == Actor::EDead)
		{
			deadActors.emplace_back(actor);
		}
	}

	// 死んだアクターを削除する(これによりmActorsから削除される)
	for (auto actor : deadActors)
	{
		delete actor;
	}

	// シーン変更は update の外で適用する (実行中のシーンを破棄しないため)
	ApplySceneRequests();
}

void Game::GenerateOutput()
{
	int drawableW = 0;
	int drawableH = 0;
	SDL_GL_GetDrawableSize(mWindow, &drawableW, &drawableH);
	if (drawableW <= 0 || drawableH <= 0)
	{
		// 最小化中など
		SDL_GL_SwapWindow(mWindow);
		return;
	}

	// レターボックスの帯も含めて全面を消す
	glViewport(0, 0, drawableW, drawableH);
	glClearColor(0.043f, 0.043f, 0.043f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// 論理解像度の比率を保って中央に表示する
	const float scale = std::fmin(drawableW / mScreenWidth, drawableH / mScreenHeight);
	if (scale != mRenderScale)
	{
		// 文字を新しい拡大率で作り直す (Text が描画時に気づく)
		mRenderScale = scale;
		for (auto& font : mFonts)
		{
			font.second->SetScale(scale);
		}
	}
	int viewW = static_cast<int>(std::lround(mScreenWidth * scale));
	int viewH = static_cast<int>(std::lround(mScreenHeight * scale));
	viewW = std::max(1, std::min(viewW, drawableW));
	viewH = std::max(1, std::min(viewH, drawableH));
	glViewport((drawableW - viewW) / 2, (drawableH - viewH) / 2, viewW, viewH);

	// 全てのスプライトコンポーネントを描画する
	// テクスチャは乗算済みアルファなのでそれに合わせたブレンドにする
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

	// シェーダ/vaoをアクティブに設定する
	mSpriteShader->SetActive();
	mSpriteVerts->SetActive();
	for (auto sprite : mSprites)
	{
		sprite->Draw(mSpriteShader);
	}

	// シーンはスプライトより手前に描く
	if (!mSceneStack.empty())
	{
		mSceneStack.top()->draw(mSpriteShader);
	}

	// バッファを交換する
	SDL_GL_SwapWindow(mWindow);
}

bool Game::LoadShaders()
{
	mSpriteShader = new Shader();
	if (!mSpriteShader->Load(mBasePath + "Shaders/Sprite.vert",
							 mBasePath + "Shaders/Sprite.frag"))
	{
		return false;
	}

	mSpriteShader->SetActive();
	// ビュー投影行列を設定する
	Matrix4 viewProj = Matrix4::CreateSimpleViewProj(mScreenWidth, mScreenHeight);
	mSpriteShader->SetMatrixUniform("uViewProj", viewProj);
	return true;
}

void Game::CreateSpriteVerts()
{
	float vertices[] = {
		-0.5f,  0.5f, 0.f, 0.f, 0.f, // 左上
		 0.5f,  0.5f, 0.f, 1.f, 0.f, // 右上
		 0.5f, -0.5f, 0.f, 1.f, 1.f, // 右下
		-0.5f, -0.5f, 0.f, 0.f, 1.f  // 左下
	};

	unsigned int indices[] = {
		0, 1, 2,
		2, 3, 0
	};

	mSpriteVerts = new VertexArray(vertices, 4, indices, 6);
}

bool Game::CreateBuiltinTextures()
{
	// 矩形描画用の 1x1 白
	SDL_Surface* white = SDL_CreateRGBSurfaceWithFormat(0, 1, 1, 32, SDL_PIXELFORMAT_RGBA32);
	if (!white)
	{
		return false;
	}
	SDL_FillRect(white, nullptr, SDL_MapRGBA(white->format, 255, 255, 255, 255));
	mWhiteTexture = new Texture();
	bool ok = mWhiteTexture->CreateFromSurface(white);
	SDL_FreeSurface(white);
	if (!ok)
	{
		return false;
	}

	// 円描画用。縁をアンチエイリアスした白い円
	const int size = 64;
	SDL_Surface* circle = SDL_CreateRGBSurfaceWithFormat(0, size, size, 32, SDL_PIXELFORMAT_RGBA32);
	if (!circle)
	{
		return false;
	}
	Uint8* row = static_cast<Uint8*>(circle->pixels);
	const float radius = size * 0.5f;
	for (int y = 0; y < size; y++)
	{
		Uint8* p = row;
		for (int x = 0; x < size; x++)
		{
			const float dx = x + 0.5f - radius;
			const float dy = y + 0.5f - radius;
			const float coverage = std::fmax(0.0f, std::fmin(1.0f, radius - std::sqrt(dx * dx + dy * dy)));
			p[0] = 255;
			p[1] = 255;
			p[2] = 255;
			p[3] = static_cast<Uint8>(coverage * 255.0f);
			p += 4;
		}
		row += circle->pitch;
	}
	mCircleTexture = new Texture();
	ok = mCircleTexture->CreateFromSurface(circle);
	SDL_FreeSurface(circle);
	return ok;
}

void Game::LoadData()
{
	// 最初のシーンはメインメニュー
	mSceneStack.push(CreateScene(EScene::MainMenu, Parameter()));
}

void Game::UnloadData()
{
	// シーンは GL コンテキストとフォントより先に破棄する
	while (!mSceneStack.empty())
	{
		mSceneStack.pop();
	}

	// アクターを削除する
	// ~ActorはRemoveActorを呼び出すので、別のスタイルのループが必要である
	while (!mActors.empty())
	{
		delete mActors.back();
	}

	// テクスチャを破棄する
	for (auto i : mTextures)
	{
		i.second->Unload();
		delete i.second;
	}
	mTextures.clear();

	for (auto i : mFonts)
	{
		i.second->Unload();
		delete i.second;
	}
	mFonts.clear();

	for (Texture* tex : { mWhiteTexture, mCircleTexture })
	{
		if (tex)
		{
			tex->Unload();
			delete tex;
		}
	}
	mWhiteTexture = nullptr;
	mCircleTexture = nullptr;
}

Texture* Game::GetTexture(const std::string& fileName)
{
	Texture* tex = nullptr;
	auto iter = mTextures.find(fileName);
	if (iter != mTextures.end())
	{
		tex = iter->second;
	}
	else
	{
		tex = new Texture();
		if (tex->Load(mBasePath + fileName))
		{
			mTextures.emplace(fileName, tex);
		}
		else
		{
			delete tex;
			tex = nullptr;
		}
	}
	return tex;
}

Font* Game::GetFont(const std::string& fileName)
{
	auto iter = mFonts.find(fileName);
	if (iter != mFonts.end())
	{
		return iter->second;
	}

	Font* font = new Font();
	font->SetScale(mRenderScale);
	if (!font->Load(mBasePath + fileName))
	{
		delete font;
		return nullptr;
	}
	mFonts.emplace(fileName, font);
	return font;
}

void Game::onSceneChanged(const EScene scene, const Parameter& parameter, const bool stackClear)
{
	mSceneRequests.push_back({ SceneRequest::Push, scene, parameter, stackClear });
}

void Game::onSceneChanged(const bool popScene)
{
	if (popScene)
	{
		mSceneRequests.push_back({ SceneRequest::Pop, EScene::MainMenu, Parameter(), false });
	}
}

void Game::ApplySceneRequests()
{
	std::vector<SceneRequest> requests;
	requests.swap(mSceneRequests);
	for (auto& req : requests)
	{
		if (req.kind == SceneRequest::Pop)
		{
			// 最後の 1 枚は残す
			if (mSceneStack.size() > 1)
			{
				mSceneStack.pop();
			}
			continue;
		}

		if (req.stackClear)
		{
			while (!mSceneStack.empty())
			{
				mSceneStack.pop();
			}
		}
		mSceneStack.push(CreateScene(req.scene, req.parameter));
	}
}

std::shared_ptr<AbstractScene> Game::CreateScene(EScene scene, const Parameter& parameter)
{
	switch (scene)
	{
	case EScene::Ranking:
		return std::make_shared<RankingScene>(this, parameter, this);
	case EScene::Typing:
		return std::make_shared<TypingScene>(this, parameter, this);
	case EScene::MainMenu:
	default:
		return std::make_shared<MainMenuScene>(this, parameter, this);
	}
}

void Game::Shutdown()
{
	UnloadData();
	TTF_Quit();
	IMG_Quit();
	delete mSpriteVerts;
	if (mSpriteShader)
	{
		mSpriteShader->Unload();
		delete mSpriteShader;
	}
	SDL_GL_DeleteContext(mContext);
	SDL_DestroyWindow(mWindow);
	SDL_Quit();
}

void Game::AddActor(Actor* actor)
{
	// アクターを更新中なら、保留中の配列に追加する必要がある
	if (mUpdatingActors)
	{
		mPendingActors.emplace_back(actor);
	}
	else
	{
		mActors.emplace_back(actor);
	}
}

void Game::RemoveActor(Actor* actor)
{
	// 保留中のアクターかどうか
	auto iter = std::find(mPendingActors.begin(), mPendingActors.end(), actor);
	if (iter != mPendingActors.end())
	{
		// 配列の一番後ろに移動しポップオフ(eraseによるコピーを避ける)
		std::iter_swap(iter, mPendingActors.end() - 1);
		mPendingActors.pop_back();
	}

	// 通常のアクターかどうか
	iter = std::find(mActors.begin(), mActors.end(), actor);
	if (iter != mActors.end())
	{
		// 配列の一番後ろに移動しポップオフ(eraseによるコピーを避ける)
		std::iter_swap(iter, mActors.end() - 1);
		mActors.pop_back();
	}
}

void Game::AddSprite(SpriteComponent* sprite)
{
	// ソートされた配列のどこに入れ込むかを調べる
	// (自分より描画順が大きい最初の要素)
	int myDrawOrder = sprite->GetDrawOrder();
	auto iter = mSprites.begin();
	for (;
		iter != mSprites.end();
		++iter)
	{
		if (myDrawOrder < (*iter)->GetDrawOrder())
		{
			break;
		}
	}

	// イテレータの位置より前に要素を挿入する
	mSprites.insert(iter, sprite);
}

void Game::RemoveSprite(SpriteComponent* sprite)
{
	auto iter = std::find(mSprites.begin(), mSprites.end(), sprite);
	mSprites.erase(iter);
}
