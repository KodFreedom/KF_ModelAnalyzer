//--------------------------------------------------------------------------------
//	�A�N�^�[
//�@gameObjectActor.h
//	Author : Xu Wenjie
//	Date   : 2017-05-22
//--------------------------------------------------------------------------------
#pragma once

//--------------------------------------------------------------------------------
//  �C���N���[�h�t�@�C��
//--------------------------------------------------------------------------------
#include "gameObject.h"

//--------------------------------------------------------------------------------
//  �O���錾
//--------------------------------------------------------------------------------
class CAnimatorComponent;

//--------------------------------------------------------------------------------
//  �N���X�錾
//--------------------------------------------------------------------------------
class CGameObjectActor : public CGameObject
{
public:
	CGameObjectActor(const GOM::PRIORITY &pri);
	~CGameObjectActor() {}

	bool Init(void) override;
	void Uninit(void) override;
	void LateUpdate(void) override;

	//Get�֐�
	auto GetAnimatorComponent(void) const { return m_pAnimator; }
	
	//Set�֐�

	//�����֐�
private:
	CAnimatorComponent* m_pAnimator;
};
