//--------------------------------------------------------------------------------
//
//　modeModelAnalyzer.h
//	Author : Xu Wenjie
//	Date   : 2017-09-15
//--------------------------------------------------------------------------------
#pragma once

//--------------------------------------------------------------------------------
//  インクルードファイル
//--------------------------------------------------------------------------------
#include "mode.h"

//--------------------------------------------------------------------------------
//  前方宣言
//--------------------------------------------------------------------------------
class CGameObject;

//--------------------------------------------------------------------------------
//  クラス宣言
//--------------------------------------------------------------------------------
class CModeModelAnalyzer : public CMode
{
public:
	CModeModelAnalyzer();
	~CModeModelAnalyzer();

	void	Init(void) override;
	void	Uninit(void) override;
	void	Update(void) override;
	void	LateUpdate(void) override;
	void	EndMode(void) override {}

	auto	GetModelAnalyzer(void) { return m_pModelAnalyzer; }

private:
	CGameObject* m_pModelAnalyzer;
};