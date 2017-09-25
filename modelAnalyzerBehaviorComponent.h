//--------------------------------------------------------------------------------
//	モデルアナライザービヘイビアコンポネント
//　modelAnalyzerBehaviorComponent.h
//	Author : Xu Wenjie
//	Date   : 2017-07-19
//--------------------------------------------------------------------------------
#pragma once

//--------------------------------------------------------------------------------
//  インクルードファイル
//--------------------------------------------------------------------------------
#include "behaviorComponent.h"

//--------------------------------------------------------------------------------
//  前方宣言
//--------------------------------------------------------------------------------
class CMyNode;
class CAnimator;

//--------------------------------------------------------------------------------
//  クラス宣言
//--------------------------------------------------------------------------------
class CModelAnalyzerBehaviorComponent : public CBehaviorComponent
{
public:
	//--------------------------------------------------------------------------------
	//  関数定義
	//--------------------------------------------------------------------------------
	CModelAnalyzerBehaviorComponent(CGameObject* const pGameObj);
	~CModelAnalyzerBehaviorComponent() {}

	bool Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void LateUpdate(void) override;

	void OnTrigger(CColliderComponent& colliderThis, CColliderComponent& collider) override {}
	void OnCollision(CCollisionInfo& collisionInfo) override {}

	auto GetRootNode(void) { return m_pRootNode; }
	void ChangeModel(const string& strFilePath);
	void SaveModel(void);

private:
	//--------------------------------------------------------------------------------
	//  関数定義
	//--------------------------------------------------------------------------------
	void		releaseModel(void);

	//ImGui
	void		showMainMenuBar(void);
	void		showMainMenuFile(void);
	void		showMainWindow(void);
	void		showModelInfoWindow(void);
	void		showNodeInfo(CMyNode* pNode);
	void		showNodeNowWindow(void);
	void		showAnimatorWindow(void);
	void		showCameraWindow(void);

	//--------------------------------------------------------------------------------
	//  変数定義
	//--------------------------------------------------------------------------------
	bool		m_bDrawNormal;
	bool		m_bReverseV;
	bool		m_bSaved;
	string		m_strFileName;
	CMyNode*	m_pRootNode;
	CAnimator*	m_pAnimator;

	//ImGui
	bool		m_bModelInfoWindow;
	bool		m_bAnimatorWindow;
	bool		m_bCameraWindow;
	CMyNode*	m_pNodeNow;
	CKFVec3		m_vNodeNowCorrectTrans;
	CKFVec3		m_vNodeNowCorrectRot;
	CKFVec3		m_vNodeNowCorrectScale;

	//Animator
	bool		m_bPlayMotion;
	int			m_nCntFrame;
	int			m_nNoMotion;
};