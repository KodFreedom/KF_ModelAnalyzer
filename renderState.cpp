//--------------------------------------------------------------------------------
//	レンダーステートクラス
//　renderState.cpp
//	Author : Xu Wenjie
//	Date   : 2017-05-22
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//  インクルードファイル
//--------------------------------------------------------------------------------
#include "main.h"
#include "renderState.h"
#include "manager.h"

#ifdef USING_DIRECTX
#include "rendererDX.h"
#endif

//--------------------------------------------------------------------------------
//  クラス
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//  LightOffクラス
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//  レンダーステート設定
//--------------------------------------------------------------------------------
void CLightOffRenderState::SetRenderState(void)
{
#ifdef USING_DIRECTX
	LPDIRECT3DDEVICE9 pDevice = CMain::GetManager()->GetRenderer()->GetDevice();
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
#endif
}

//--------------------------------------------------------------------------------
//  レンダーステートリセット
//--------------------------------------------------------------------------------
void CLightOffRenderState::ResetRenderState(void)
{
#ifdef USING_DIRECTX
	LPDIRECT3DDEVICE9 pDevice = CMain::GetManager()->GetRenderer()->GetDevice();
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
#endif
}

//--------------------------------------------------------------------------------
//  LightOffCullOffクラス
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//  レンダーステート設定
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
//  レンダーステートリセット
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
//  LightOffCullOnクラス
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//  レンダーステート設定
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
//  レンダーステートリセット
//--------------------------------------------------------------------------------
void CLightOffCullOnRenderState::ResetRenderState(void)
{
#ifdef USING_DIRECTX
	LPDIRECT3DDEVICE9 pDevice = CMain::GetManager()->GetRenderer()->GetDevice();
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
#endif
}

//--------------------------------------------------------------------------------
//  LightOnCullOffクラス
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//  レンダーステート設定
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
//  レンダーステートリセット
//--------------------------------------------------------------------------------
void CLightOnCullOffRenderState::ResetRenderState(void)
{
#ifdef USING_DIRECTX
	LPDIRECT3DDEVICE9 pDevice = CMain::GetManager()->GetRenderer()->GetDevice();
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
#endif
}

//--------------------------------------------------------------------------------
//  LightOffクラス
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//  レンダーステート設定
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
//  レンダーステートリセット
//--------------------------------------------------------------------------------
void CLightOnCullOnRenderState::ResetRenderState(void)
{
#ifdef USING_DIRECTX

#endif
}