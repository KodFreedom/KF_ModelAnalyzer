//--------------------------------------------------------------------------------
//	描画コンポネント
//　drawComponent.cpp
//	Author : Xu Wenjie
//	Date   : 2017-05-18	
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//  インクルードファイル
//--------------------------------------------------------------------------------
#include "drawComponent.h"
#include "manager.h"
#include "textureManager.h"
#include "materialManager.h"

//--------------------------------------------------------------------------------
//  静的メンバ変数
//--------------------------------------------------------------------------------
CLightOffRenderState	CDrawComponent::s_lightOffRenderState;
CLightOffCullOffRenderState	CDrawComponent::s_lightOffCullOff;
CLightOffCullOnRenderState	CDrawComponent::s_lightOffCullOn;
CLightOnCullOffRenderState	CDrawComponent::s_lightOnCullOff;
CLightOnCullOnRenderState	CDrawComponent::s_lightOnCullOn;
CNullRenderState		CDrawComponent::s_nullRenderState;

//--------------------------------------------------------------------------------
//	クラス
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//  終了処理
//--------------------------------------------------------------------------------
void CDrawComponent::Uninit(void)
{
	if (!m_strTexName.empty())
	{
		CMain::GetManager()->GetTextureManager()->DisuseTexture(m_strTexName);
		m_strTexName.clear();
	}
}

//--------------------------------------------------------------------------------
//  テクスチャ設定
//--------------------------------------------------------------------------------
void CDrawComponent::SetTexName(const string& strTexName)
{
	m_strTexName = strTexName;
	CMain::GetManager()->GetTextureManager()->UseTexture(m_strTexName);
}