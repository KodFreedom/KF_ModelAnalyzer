//--------------------------------------------------------------------------------
//	モデルアナライザー描画コンポネント
//　modelAnalyzerDrawComponent.cpp
//	Author : Xu Wenjie
//	Date   : 2017-05-21	
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//  インクルードファイル
//--------------------------------------------------------------------------------
#include "modelAnalyzerBehaviorComponent.h"
#include "modelAnalyzerDrawComponent.h"
#include "gameObject.h"
#include "KF_UtilityFBX.h"

//--------------------------------------------------------------------------------
//  クラス
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//  描画処理
//--------------------------------------------------------------------------------
void CModelAnalyzerDrawComponent::Draw(void)
{
	auto pRootNode = c_pMA->GetRootNode();
	if (!pRootNode) { return; }
	auto mtxWorld = m_pGameObj->GetTransformComponent()->GetMatrix();
	m_pRenderState->SetRenderState();
	pRootNode->RecursiveDraw(m_bDrawNormal, mtxWorld);
	m_pRenderState->ResetRenderState();
}
