//--------------------------------------------------------------------------------
//	モデルアナライザー描画コンポネント
//　modelAnalyzerDrawComponent.h
//	Author : Xu Wenjie
//	Date   : 2017-05-21	
//--------------------------------------------------------------------------------
#pragma once

//--------------------------------------------------------------------------------
//  インクルードファイル
//--------------------------------------------------------------------------------
#include "drawComponent.h"

//--------------------------------------------------------------------------------
//  前方宣言
//--------------------------------------------------------------------------------
class CModelAnalyzerBehaviorComponent;

//--------------------------------------------------------------------------------
//  クラス宣言
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//  3D描画コンポネントクラス
//--------------------------------------------------------------------------------
class CModelAnalyzerDrawComponent : public CDrawComponent
{
public:
	CModelAnalyzerDrawComponent(CModelAnalyzerBehaviorComponent* const pMA, CGameObject* const pGameObj)
		: CDrawComponent(pGameObj)
		, c_pMA(pMA)
		, m_bDrawNormal(false) {}
	~CModelAnalyzerDrawComponent() {}

	void	Draw(void) override;
	void	SetDrawNormalFlag(const bool& bFlag) { m_bDrawNormal = bFlag; }

private:
	CModelAnalyzerBehaviorComponent* const c_pMA;
	bool m_bDrawNormal;
};