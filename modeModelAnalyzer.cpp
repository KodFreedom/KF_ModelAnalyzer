//--------------------------------------------------------------------------------
//
//　modeEditor.cpp
//	Author : Xu Wenjie
//	Date   : 2017-08-19
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//  インクルードファイル
//--------------------------------------------------------------------------------
#include "main.h"
#include "manager.h"
#include "lightManager.h"
#include "inputManager.h"
#include "modeModelAnalyzer.h"
#include "modelAnalyzerCamera.h"

//gameobject
#include "gameObjectSpawner.h"
#include "gameObjectActor.h"

//--------------------------------------------------------------------------------
//  クラス
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//  コンストラクタ
//--------------------------------------------------------------------------------
CModeModelAnalyzer::CModeModelAnalyzer()
	: CMode()
	, m_pModelAnalyzer(nullptr)
{

}

//--------------------------------------------------------------------------------
//  デストラクタ
//--------------------------------------------------------------------------------
CModeModelAnalyzer::~CModeModelAnalyzer()
{

}

//--------------------------------------------------------------------------------
//  初期化処理
//--------------------------------------------------------------------------------
void CModeModelAnalyzer::Init(void)
{
	//ライトの初期化
	CMain::GetManager()->GetLightManager()->CreateDirectionalLight(CKFVec3(0.5f, -0.5f, 0.5f));

	//カメラの初期化
	m_pCamera = new CModelAnalyzerCamera;
	m_pCamera->Init();

	CMain::GetManager()->GetInputManager()->SetEditorMode(true);

	//ゲームオブジェクトの初期化
	CGameObjectSpawner::CreateGrid();
	m_pModelAnalyzer = CGameObjectSpawner::CreateModelAnalyzer();
	//CGameObject3D::CreateSkyBox(CKFVec3(0.0f), CKFVec3(0.0f), CKFVec3(1.0f));
}

//--------------------------------------------------------------------------------
//  終了処理
//--------------------------------------------------------------------------------
void CModeModelAnalyzer::Uninit(void)
{
	//カメラとゲームオブジェクトの破棄
	CMode::Uninit();

	//ライトの破棄
	CMain::GetManager()->GetLightManager()->ReleaseAll();

	CMain::GetManager()->GetInputManager()->SetEditorMode(false);
}

//--------------------------------------------------------------------------------
//  更新処理
//--------------------------------------------------------------------------------
void CModeModelAnalyzer::Update(void)
{
	CMode::Update();
}

//--------------------------------------------------------------------------------
//  更新処理(描画直前)
//--------------------------------------------------------------------------------
void CModeModelAnalyzer::LateUpdate(void)
{
	CMode::LateUpdate();
}