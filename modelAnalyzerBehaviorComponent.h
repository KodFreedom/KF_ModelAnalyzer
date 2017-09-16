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

//--------------------------------------------------------------------------------
//  �O���錾
//--------------------------------------------------------------------------------
class CMyNode;

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

private:
	//--------------------------------------------------------------------------------
	//  �֐���`
	//--------------------------------------------------------------------------------
	void		releaseModel(void);

	//--------------------------------------------------------------------------------
	//  �ϐ���`
	//--------------------------------------------------------------------------------
	bool		m_bDrawNormal;
	bool		m_bEnableCullFace;
	bool		m_bEnableLight;
	bool		m_bReverseV;
	bool		m_bSaved;
	string		m_strFileName;
	CMyNode*	m_pRootNode;
};