#include "GameScene.h"

using namespace KamataEngine;

GameScene::~GameScene() { Model2::StaticFinalize(); }

void GameScene::Initialize() {
	Model2::StaticInitialize();

	// model_ = Model2::CreateSquare(5); // ←四角形
	model_ = Model2::CreateRing(8);
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
	Model2::PreDraw(cmdList);

	// モデル描画
	model_->Draw(worldTransform_, camera_, textureHandle_);

	// 描画終了
	Model2::PostDraw();
}