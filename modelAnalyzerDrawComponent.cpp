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
#include "manager.h"
#include "rendererDX.h"

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
	auto pDevice = CMain::GetManager()->GetRenderer()->GetDevice();
	pRootNode->RecursiveDraw(c_pMA->IsDrawSkeleton(), c_pMA->IsDrawMesh(), c_pMA->IsDrawCollider());
}
