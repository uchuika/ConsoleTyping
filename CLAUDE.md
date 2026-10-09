# ConsoleTyping

文化祭で発表するターミナル風タイピングゲーム。仕様は `PLAN.md` にあり、原則これに従って開発する。

## 技術基盤

C++ / SDL2 / OpenGL

## 企画の要点 (詳細は PLAN.md)

- PowerShell 風の画面。配色は CLI 版 Grok のデザインを基にする。
- 1 分間のローマ字タイピング。誤キーでは進まない (寿司打方式)。Shift などの修飾キーとキーリピートは無視する。
- 画面はメインメニュー、ゲーム、ランキングの 3 つ。ゲーム開始前にニックネーム入力 (既定値 `Player`) と 3 カウントがある。
- 問題は `problems.txt` (1 行 1 問、出題順は固定)。禁止語は `ng-words.txt`。
- スコアは `scores.json` に保存する。一時ファイルに書いてから置き換える。
- ランキングは打てた文字数の降順で、上位 20 位まで上下キーで見られる。
- ローマ字は多くの入力パターンに対応する。長音は `-` キーで打つ。
- 操作: 上下キーで選択、Enter で決定、ESC で戻る。タイピング中の ESC は確認を出してメインメニューへ戻る。
- Alt+Enter でボーダレスフルスクリーンとウィンドウ表示を切り替える。ウィンドウの閉じるボタンで終了する。
- 未決: IME を無視して入力を受け付ける方法。

## 設計・コーディングの参考

次の 2 つのオブジェクト指向の考え方とコードの書き方に合わせる。

### `D:\C++Game\GameProgrammingC++code\Chapter05` (Madhav 書籍のサンプル)

- `Game` が `std::vector<Actor*>` と保留中の `mPendingActors` を持ち、更新中は追加を保留する。
- `Actor` は状態 (`EActive` / `EPaused` / `EDead`) と `std::vector<Component*>` を持つ。
  - `Update` / `ProcessInput` は非 virtual。派生側は `UpdateActor` / `ActorInput` をオーバーライドする。
- `Component(owner, updateOrder)`。updateOrder の小さい順に更新する。
- `SpriteComponent` は `Game` のスプライト一覧に登録して描画する。
- OpenGL ラッパーは `Shader` / `VertexArray` / `Texture`。
- 規約: メンバー変数は `m` 接頭辞、Allman 形式の波括弧、タブインデント、ヘッダでは `class X*` で前方宣言する。

### `D:\C++Game\ProjectWhite\ProjectWhite` (自作。Chapter05 の拡張)

- `AbstractScene` は純粋仮想の `update()` / `draw()` / `GetScene()` を持つ。`Game` が `std::stack<std::shared_ptr<AbstractScene>>` で管理する。
- シーン遷移は `IOnSceneChangedListener::onSceneChanged(EScene, Parameter, stackClear)` と `onSceneChanged(bool popScene)` で行う。
- `Singleton<T>` の `getIns()` を使う。例: `Keyboard` は各キーの押下/解放フレーム数を数える。
- `UIScreen` のスタックと `Button` (`std::function<void()>` のクリック処理)。
- `Font` / `Text` のラッパー。`Parameter` / `Define` / `Struct` / `Macro` / `EScreen` などの共通ヘッダ。
- 一部の private メンバーに `_member` 形式を使う。
- **ソースが CP932 (Shift-JIS) のため、UTF-8 として読むと日本語コメントが化ける。** 外部ツール (grok など) に渡す前に UTF-8 へ変換したコピーを使う。

## 方針

- 新規コードは上記 2 つと同じ構成 (シーンスタック、Actor/Component、Singleton の入力管理) と書式に合わせる。
- 仕様の変更や PLAN.md との食い違いは、実装前にユーザーへ確認する。
