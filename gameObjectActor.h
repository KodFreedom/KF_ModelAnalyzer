//--------------------------------------------------------------------------------
//	アクター
//　gameObjectActor.h
//	Author : Xu Wenjie
//	Date   : 2017-05-22
//--------------------------------------------------------------------------------
#pragma once

//--------------------------------------------------------------------------------
//  インクルードファイル
//--------------------------------------------------------------------------------
#include "gameObject.h"

//--------------------------------------------------------------------------------
//  前方宣言
//--------------------------------------------------------------------------------
class CAnimatorComponent;

//--------------------------------------------------------------------------------
//  クラス宣言
//--------------------------------------------------------------------------------
class CGameObjectActor : public CGameObject
{
public:
	CGameObjectActor(const GOM::PRIORITY &pri);
	~CGameObjectActor() {}

	bool Init(void) override;
	void Uninit(void) override;
	void LateUpdate(void) override;

	//Get関数
	auto GetAnimatorComponent(void) const { return m_pAnimator; }
	
	//Set関数

	//生成関数
private:
	CAnimatorComponent* m_pAnimator;
};
