//--------------------------------------------------------------------------------
//	���f���A�i���C�U�[�`��R���|�l���g
//�@modelAnalyzerDrawComponent.cpp
//	Author : Xu Wenjie
//	Date   : 2017-05-21	
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//  �C���N���[�h�t�@�C��
//--------------------------------------------------------------------------------
#include "modelAnalyzerBehaviorComponent.h"
#include "modelAnalyzerDrawComponent.h"
#include "gameObject.h"
#include "KF_UtilityFBX.h"
#include "manager.h"
#include "rendererDX.h"

//--------------------------------------------------------------------------------
//  �N���X
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//  �`�揈��
//--------------------------------------------------------------------------------
void CModelAnalyzerDrawComponent::Draw(void)
{
	auto pRootNode = c_pMA->GetRootNode();
	if (!pRootNode) { return; }
	auto mtxWorld = m_pGameObj->GetTransformComponent()->GetMatrix();
	auto pDevice = CMain::GetManager()->GetRenderer()->GetDevice();
	pRootNode->RecursiveDraw(m_bDrawNormal, mtxWorld);
}
