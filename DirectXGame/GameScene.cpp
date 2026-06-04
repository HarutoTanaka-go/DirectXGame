#include "GameScene.h"

using namespace KamataEngine;

GameScene::~GameScene() { Effect::StaticFinalize(); }

void GameScene::Initialize() {
	Effect::StaticInitialize();

	// effect_ = Effect::CreateSquare(1); // ←四角形 ()内の数字を増やすと個数が増え、四角形がぴったり並んで配置される
	/*effect_ = Effect::CreateRing(8);*/
	effect_ = Effect::CreateRhombus(1); // 菱形
	worldTransform_.Initialize();
	camera_.Initialize();

	textureHandle_ = TextureManager::Load("uvChecker.png");
}

void GameScene::Update() {
	worldTransform_.TransferMatrix();
	camera_.TransferMatrix();
}

void GameScene::Draw() {
	ID3D12GraphicsCommandList* cmdList = DirectXCommon::GetInstance()->GetCommandList();

	// 描画開始
	Effect::PreDraw(cmdList);

	// モデル描画
	effect_->Draw(worldTransform_, camera_, textureHandle_);

	// 描画終了
	Effect::PostDraw();
}