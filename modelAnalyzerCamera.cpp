//--------------------------------------------------------------------------------
//	���f���A�i���C�U�[�p�J�����N���X
//�@modelAnalyzerCamera.h
//	Author : Xu Wenjie
//	Date   : 2016-06-20
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//  �C���N���[�h�t�@�C��
//--------------------------------------------------------------------------------
#include "main.h"
#include "manager.h"
#include "inputManager.h"
#include "modelAnalyzerCamera.h"

//--------------------------------------------------------------------------------
//  �N���X
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//  �ÓI�����o�ϐ�
//--------------------------------------------------------------------------------
const float	CModelAnalyzerCamera::sc_fRotSpeed = 0.05f;
const float	CModelAnalyzerCamera::sc_fStartRotMin = 0.2f;
const float	CModelAnalyzerCamera::sc_fZoomSpeed = 0.5f;
const float	CModelAnalyzerCamera::sc_fDistanceMin = 3.0f;
const float	CModelAnalyzerCamera::sc_fDistanceMax = 500.0f;

//--------------------------------------------------------------------------------
//
//  public
//
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//  �R���X�g���N�^
//--------------------------------------------------------------------------------
CModelAnalyzerCamera::CModelAnalyzerCamera() : CCamera()
{
}

//--------------------------------------------------------------------------------
//  ����������
//--------------------------------------------------------------------------------
void CModelAnalyzerCamera::Init(void)
{
	CCamera::Init();
	SetCamera(CKFVec3(0.0f), CKFVec3(0.0f, 10.0f, -10.0f), CKFVec3(0.0f, 1.0f, 0.0f), CKFVec3(1.0f, 0.0f, 0.0f));
}

//--------------------------------------------------------------------------------
//  �X�V����
//--------------------------------------------------------------------------------
void CModelAnalyzerCamera::Update(void)
{
	auto pInput = CMain::GetManager()->GetInputManager();
	CKFVec3 vRot = CKFVec3(0.0f);
	float fZoomSpeed = 0.0f;
	float fAxisX = pInput->GetMoveHorizontal();
	float fAxisY = pInput->GetMoveVertical();
	float fRAxisX = pInput->GetRotHorizontal();
	float fRAxisY = pInput->GetRotVertical();
	float fZoom = pInput->GetZoom();

	//���ړ_��]
	if (fabsf(fRAxisX) > sc_fStartRotMin)
	{//Y����]
		vRot.m_fY = sc_fRotSpeed * fRAxisX;
	}
	if (fabsf(fRAxisY) > sc_fStartRotMin)
	{//X����]
		vRot.m_fX = sc_fRotSpeed * fRAxisY;
	}

	//�g��k��
	if (fabsf(fZoom) > sc_fStartRotMin)
	{
		fZoomSpeed = sc_fZoomSpeed * fZoom;
	}

	//��]
	Yaw(vRot.m_fY);
	Pitch(vRot.m_fX);

	//�Y�[��
	m_fDistance += fZoomSpeed;
	m_fDistance = m_fDistance < sc_fDistanceMin ? sc_fDistanceMin : m_fDistance > sc_fDistanceMax ? sc_fDistanceMax : m_fDistance;
}