//--------------------------------------------------------------------------------
//	�����_�[�X�e�[�g�N���X
//�@renderState.cpp
//	Author : Xu Wenjie
//	Date   : 2017-05-22
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//  �C���N���[�h�t�@�C��
//--------------------------------------------------------------------------------
#include "main.h"
#include "renderState.h"
#include "manager.h"

#ifdef USING_DIRECTX
#include "rendererDX.h"
#endif

//--------------------------------------------------------------------------------
//  �N���X
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//  LightOff�N���X
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//  �����_�[�X�e�[�g�ݒ�
//--------------------------------------------------------------------------------
void CLightOffRenderState::SetRenderState(void)
{
#ifdef USING_DIRECTX
	LPDIRECT3DDEVICE9 pDevice = CMain::GetManager()->GetRenderer()->GetDevice();
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
#endif
}

//--------------------------------------------------------------------------------
//  �����_�[�X�e�[�g���Z�b�g
//--------------------------------------------------------------------------------
void CLightOffRenderState::ResetRenderState(void)
{
#ifdef USING_DIRECTX
	LPDIRECT3DDEVICE9 pDevice = CMain::GetManager()->GetRenderer()->GetDevice();
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
#endif
}

//--------------------------------------------------------------------------------
//  LightOffCullOff�N���X
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//  �����_�[�X�e�[�g�ݒ�
//--------------------------------------------------------------------------------
void CLightOffCullOffRenderState::SetRenderState(void)
{
#ifdef USING_DIRECTX
	LPDIRECT3DDEVICE9 pDevice = CMain::GetManager()->GetRenderer()->GetDevice();
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
#endif
}

//--------------------------------------------------------------------------------
//  �����_�[�X�e�[�g���Z�b�g
//--------------------------------------------------------------------------------
void CLightOffCullOffRenderState::ResetRenderState(void)
{
#ifdef USING_DIRECTX
	LPDIRECT3DDEVICE9 pDevice = CMain::GetManager()->GetRenderer()->GetDevice();
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
#endif
}

//--------------------------------------------------------------------------------
//  LightOffCullOn�N���X
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//  �����_�[�X�e�[�g�ݒ�
//--------------------------------------------------------------------------------
void CLightOffCullOnRenderState::SetRenderState(void)
{
#ifdef USING_DIRECTX
	LPDIRECT3DDEVICE9 pDevice = CMain::GetManager()->GetRenderer()->GetDevice();
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
#endif
}

//--------------------------------------------------------------------------------
//  �����_�[�X�e�[�g���Z�b�g
//--------------------------------------------------------------------------------
void CLightOffCullOnRenderState::ResetRenderState(void)
{
#ifdef USING_DIRECTX
	LPDIRECT3DDEVICE9 pDevice = CMain::GetManager()->GetRenderer()->GetDevice();
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
#endif
}

//--------------------------------------------------------------------------------
//  LightOnCullOff�N���X
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//  �����_�[�X�e�[�g�ݒ�
//--------------------------------------------------------------------------------
void CLightOnCullOffRenderState::SetRenderState(void)
{
#ifdef USING_DIRECTX
	LPDIRECT3DDEVICE9 pDevice = CMain::GetManager()->GetRenderer()->GetDevice();
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
#endif
}

//--------------------------------------------------------------------------------
//  �����_�[�X�e�[�g���Z�b�g
//--------------------------------------------------------------------------------
void CLightOnCullOffRenderState::ResetRenderState(void)
{
#ifdef USING_DIRECTX
	LPDIRECT3DDEVICE9 pDevice = CMain::GetManager()->GetRenderer()->GetDevice();
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
#endif
}

//--------------------------------------------------------------------------------
//  LightOff�N���X
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//  �����_�[�X�e�[�g�ݒ�
//--------------------------------------------------------------------------------
void CLightOnCullOnRenderState::SetRenderState(void)
{
#ifdef USING_DIRECTX
	LPDIRECT3DDEVICE9 pDevice = CMain::GetManager()->GetRenderer()->GetDevice();
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
#endif
}

//--------------------------------------------------------------------------------
//  �����_�[�X�e�[�g���Z�b�g
//--------------------------------------------------------------------------------
void CLightOnCullOnRenderState::ResetRenderState(void)
{
#ifdef USING_DIRECTX

#endif
}