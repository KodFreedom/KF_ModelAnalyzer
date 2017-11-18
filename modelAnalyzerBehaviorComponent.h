//--------------------------------------------------------------------------------
//	���f���A�i���C�U�[�r�w�C�r�A�R���|�l���g
//�@modelAnalyzerBehaviorComponent.h
//	Author : Xu Wenjie
//	Date   : 2017-07-19
//--------------------------------------------------------------------------------
#pragma once

//--------------------------------------------------------------------------------
//  �C���N���[�h�t�@�C��
//--------------------------------------------------------------------------------
#include "behaviorComponent.h"
#include "KF_UtilityFBX.h"
//--------------------------------------------------------------------------------
//  �O���錾
//--------------------------------------------------------------------------------
class CMyNode;
class CAnimator;

//--------------------------------------------------------------------------------
//  �N���X�錾
//--------------------------------------------------------------------------------
class CModelAnalyzerBehaviorComponent : public CBehaviorComponent
{
public:
	//--------------------------------------------------------------------------------
	//  �֐���`
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
	const bool& IsDrawSkeleton(void) { return m_bDrawSkeleton; }
	const bool& IsDrawMesh(void) { return m_bDrawMesh; }
	const bool& IsDrawCollider(void) { return m_bDrawCollider; }

private:
	//--------------------------------------------------------------------------------
	//  �֐���`
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
	void		changeTexture(Mesh& mesh);
	void		addAnimation(void);

	//--------------------------------------------------------------------------------
	//  �ϐ���`
	//--------------------------------------------------------------------------------
	bool		m_bDrawSkeleton;
	bool		m_bDrawMesh;
	bool		m_bDrawCollider;
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