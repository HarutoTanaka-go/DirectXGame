#include "GameScene.h"
#include "Model2.h"

using namespace KamataEngine;

GameScene::~GameScene() { delete model2_; }

void GameScene::Initialize() {

	model2_ = new Model2();
	// モデル
	model2_->StaticInitialize();
}

void GameScene::Update() {}

void GameScene::Draw() {}