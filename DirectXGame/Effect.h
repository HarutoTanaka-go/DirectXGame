#pragma once
//#include <3d\LightGroup.h>
//#include <3d\Material.h>
//#include <3d\Mesh.h>
//#include <3d\ObjectColor.h>
#include <KamataEngine.h>
#include <string>
#include <unordered_map>
#include <vector>

using namespace KamataEngine;
//{

class EffectCommon {
public:
	static EffectCommon* GetInstance();
	static void Terminate();

	/// <summary>
	///	初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// ライトコマンドを積む
	/// </summary>
	void LightCommand(const LightGroup* lightGroup = nullptr);

	/// <summary>
	/// 　トランスフォームコマンドを積む
	/// </summary>
	/// <param name="worldTransform">ワールドトランスフォーム</param>
	/// <param name="camera">カメラ</param>
	void TransformCommand(const WorldTransform* worldTransform, const Camera* camera);

	/// <summary>
	/// 描画前処理
	/// </summary>
	/// <param name="commandList">描画コマンドリスト</param>
	void PreDraw(ID3D12GraphicsCommandList* commandList);

	/// <summary>
	/// 描画後処理
	/// </summary>
	void PostDraw();

	/// <summary>
	/// getter
	/// </summary>
	ID3D12GraphicsCommandList* GetCommandList() const { return commandList_; }
	ObjectColor* GetObjectColor() const { return defaultObjectColor_.get(); }

private:
	EffectCommon() = default;
	~EffectCommon() = default;
	EffectCommon(EffectCommon&) = delete;
	EffectCommon& operator=(EffectCommon&) = delete;

	/// <summary>
	/// グラフィックスパイプライン初期化
	/// </summary>
	void InitializeGraphicsPipeline();

	// シングルトンインスタンス
	static EffectCommon* sInstance_;

	// デスクリプタサイズ
	UINT descriptorHandleIncrementSize_ = 0u;
	// コマンドリスト
	ID3D12GraphicsCommandList* commandList_ = nullptr;
	// ルートシグネチャ
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
	// パイプラインステートオブジェクト
	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState_;
	// デフォルトライト
	std::unique_ptr<LightGroup> defaultLightGroup_;
	// デフォルトオブジェクトα
	std::unique_ptr<ObjectColor> defaultObjectColor_;
};

class Effect {

public: // 列挙子
	/// <summary>
	/// ルートパラメーター
	/// </summary>
	enum class RoomParameter {
		kWorldTransform, // ワールド変換行列
		kCamera,         // カメラ
		kMaterial,       // マテリアル
		kTexture,        // テクスチャ
		kLight,          // ライト
		kObjectColor,    // オブジェクトアルファ
	};

	/// <summary>
	/// ヒット演出エフェクト
	/// </summary>
	enum class State {
		kSpread, // 拡大中
		kFade,   // フェードアウト中
		kDead    // 死亡
	};

	static Effect* Create(const KamataEngine::Vector3& position);

	bool IsDead() const { return state_ == State::kDead; }

private:
	static const char* kBaseDirectory;
	static const char* kDefaultModelName;

public: // 静的メンバ関数
	/// <summary>
	/// ゲーム中一度だけ実行する初期化
	/// </summary>
	static void StaticInitialize();

	/// <summary>
	/// ゲーム中一度だけ実行する終了処理
	/// </summary>
	static void StaticFinalize();

	/// <summary>
	/// 3Dモデル生成
	/// </summary>
	/// <returns></returns>
	static Effect* Create();

	/// <summary>
	/// OBJファイルからメッシュ生成
	/// </summary>
	/// <param name="modelname">モデル名</param>
	/// <param name="modelname">エッジ平滑化フラグ</param>
	/// <returns>生成されたモデル</returns>
	static Effect* CreateFromOBJ(const std::string& modelname, bool smoothing = false);

	/// <summary>
	/// 球モデル生成
	/// </summary>
	/// <param name="divisionVertial">垂直方向（緯度）分割数</param>
	/// <param name="divisionHorizontal">水平方向（経度）分割数</param>
	/// <returns>生成されたモデル</returns>
	static Effect* CreateSphere(uint32_t divisionVertial = 10, uint32_t divisionHorizontal = 10);

	static Effect* CreateSquare();

	/// <summary>
	/// 描画前処理
	/// </summary>
	/// <param name="commandList">描画コマンドリスト</param>
	static void PreDraw(ID3D12GraphicsCommandList* commandList);

	/// <summary>
	/// 描画後処理
	/// </summary>
	static void PostDraw();

public: // メンバ関数
	~Effect() = default;

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="worldTransform"></param>
	void Update(WorldTransform& worldTransform);

	/// <summary>
	/// 描画（テクスチャ差し替え）
	/// </summary>
	/// <param name="worldTransform">ワールドトランスフォーム</param>
	/// <param name="camera">カメラ</param>
	/// <param name="textureHadle">テクスチャハンドル</param>
	/// <param name="objectColor">オブジェクトカラー</param>
	void Draw(const WorldTransform& worldTransform, const Camera& camera, const ObjectColor* objectColor = nullptr);
	/// <summary>
	/// メッシュコンテナを取得
	/// </summary>
	/// <returns>メッシュコンテナ</returns>
	inline const std::vector<std::unique_ptr<Mesh>>& GetMeshes() { return meshes_; }
	/// <summary>
	/// 全マテリアルにアルファ値を設定する
	/// </summary>
	/// <param name="alpha"></param>
	void SetAlpha(float alpha);
	/// <summary>
	/// ライトグループを設定する
	/// </summary>
	/// <param name="lightGroup">ライトグループ</param>
	void SetLightGroup(const LightGroup* lightGroup) { lightGroup_ = lightGroup; }

private:
	// 名前
	std::string name_;
	// メッシュコンテナ
	std::vector<std::unique_ptr<Mesh>> meshes_;
	// マテリアルコンテナ
	std::unordered_map<std::string, std::unique_ptr<Material>> materials_;
	// デフォルトマテリアル
	std::unique_ptr<Material> defaultMaterial_ = nullptr;
	const LightGroup* lightGroup_ = nullptr;

	State state_ = State::kSpread;
	// カウンター
	uint32_t counter_ = 0;

	// 拡大アニメーションの時間
	static inline const uint32_t kSpreadTime = 10;

	// フェードアウトアニメーションの時間
	static inline const uint32_t kFadeTime = 20;

private: // メンバ関数
	/// <summary>
	/// ファイルを読み込んで初期化
	/// </summary>
	/// <param name="modelname">モデル名</param>
	/// <param name="smoothing">エッジ平滑化フラグ</param>
	void InitializeFromFile(const std::string& modelname, bool smoothing = false);

	/// <summary>
	/// 頂点データを渡して初期化
	/// </summary>
	/// /// <param name="vertices">頂点配列</param>
	/// <param name="indices">インデックス配列</param>
	void InitializeFromVertices(const std::vector<Mesh::VertexPosNormalUv>& vertices, const std::vector<uint32_t>& indices);

	/// <summary>
	/// モデル読み込み
	/// </summary>
	/// <param name="modelname">モデル名</param>
	/// <param name="modelname">エッジ平滑化フラグ</param>
	void LoadModel(const std::string& modelname, bool smoothing);

	/// <summary>
	/// マテリアル読み込み
	/// </summary>
	void LoadMaterial(const std::string& directoryPath, const std::string& filename);

	/// <summary>
	/// マテリアル登録
	/// </summary>
	void AddMaterial(std::unique_ptr<Material>& material);

	/// <summary>
	/// テクスチャ読み込み
	/// </summary>
	void LoadTextures();
};

//}