//--------------------------------------------------------------------------------
//	モデルアナライザービヘイビアコンポネント
//　modelAnalyzerBehaviorComponent.cpp
//	Author : Xu Wenjie
//	Date   : 2017-07-19
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//  インクルードファイル
//--------------------------------------------------------------------------------
#include "modelAnalyzerBehaviorComponent.h"
#include "gameObject.h"
#include "transformComponent.h"
#include "KF_Math.h"
#include "KF_UtilityFBX.h"

//--------------------------------------------------------------------------------
//  クラス
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//  コンストラクタ
//--------------------------------------------------------------------------------
CModelAnalyzerBehaviorComponent::CModelAnalyzerBehaviorComponent(CGameObject* const pGameObj)
	: CBehaviorComponent(pGameObj)
	, m_bDrawNormal(false)
	, m_bEnableCullFace(false)
	, m_bEnableLight(false)
	, m_bReverseV(false)
	, m_bSaved(false)
	, m_pRootNode(nullptr)
{
	m_strFileName.clear();
}

//--------------------------------------------------------------------------------
//  初期化
//--------------------------------------------------------------------------------
bool CModelAnalyzerBehaviorComponent::Init(void)
{
	m_strFileName.clear();
	m_bSaved = false;
	m_bReverseV = false;
	auto pTrans = m_pGameObj->GetTransformComponent();
	pTrans->SetPosNext(CKFMath::sc_vZero);
	pTrans->SetForwardNext(CKFMath::sc_vForward);
	pTrans->SetRightNext(CKFMath::sc_vRight);
	pTrans->SetUpNext(CKFMath::sc_vUp);
	pTrans->SetScaleNext(CKFVec3(0.05f)/*CKFMath::sc_vOne*/);
	return true;
}

//--------------------------------------------------------------------------------
//  終了処理
//--------------------------------------------------------------------------------
void CModelAnalyzerBehaviorComponent::Uninit(void)
{
	releaseModel();
}

//--------------------------------------------------------------------------------
//  更新
//--------------------------------------------------------------------------------
void CModelAnalyzerBehaviorComponent::Update(void)
{
	if (m_pRootNode)
	{
		m_pRootNode->RecursiveUpdate();
	}
}

//--------------------------------------------------------------------------------
//  更新
//--------------------------------------------------------------------------------
void CModelAnalyzerBehaviorComponent::LateUpdate(void)
{

}

//--------------------------------------------------------------------------------
// ChangeModel
//--------------------------------------------------------------------------------
void CModelAnalyzerBehaviorComponent::ChangeModel(const string& strFilePath)
{
	//Check File
	string strName, strType;
	CKFUtility::AnalyzeTexPath(strFilePath, strName, strType);
	if (strType._Equal("FBX")
		|| strType._Equal("fbx")
		|| strType._Equal("obj")
		|| strType._Equal("OBJ"))
	{
		//フルスクリーンモード確認
		auto nID = MessageBox(NULL, "モデルを切り替えますか？", "確認", MB_YESNO | MB_ICONWARNING | MB_DEFBUTTON2);

		//押し判定
		if (nID == IDNO) { return; }

		//前のモデルの削除
		releaseModel();

		//LoadModel
		m_strFileName = strName;
		m_pRootNode = CKFUtilityFBX::Load(strFilePath);
		m_pRootNode->RecursiveRecalculateVtx();
	}
	else
	{//対応してない
		MessageBox(NULL, strFilePath.c_str(), "対応できないタイプ", MB_OK);
	}
}

//--------------------------------------------------------------------------------
// releaseModel
//--------------------------------------------------------------------------------
void CModelAnalyzerBehaviorComponent::releaseModel(void)
{
	if (!m_pRootNode) { return; }

	if (!m_bSaved)
	{
		//フルスクリーンモード確認
		auto nID = MessageBox(NULL, "セーブしますか？", "確認", MB_YESNO | MB_ICONWARNING | MB_DEFBUTTON2);

		//押し判定
		if (nID == IDYES) { /*saveModelAs(m_strFileName);*/ }
	}

	SAFE_RELEASE(m_pRootNode);
	Init();
}