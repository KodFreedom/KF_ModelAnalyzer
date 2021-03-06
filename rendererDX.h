//--------------------------------------------------------------------------------
//
//　rendererDX.h
//	Author : Xu Wenjie
//	Date   : 2016-05-31
//--------------------------------------------------------------------------------
#pragma once

#ifdef USING_DIRECTX
//--------------------------------------------------------------------------------
//  インクルードファイル
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//  定数定義
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//  プロトタイプ宣言
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//  クラス宣言
//--------------------------------------------------------------------------------
class CRendererDX
{
public:
	//--------------------------------------------------------------------------------
	//  関数定義
	//--------------------------------------------------------------------------------
	CRendererDX();
	~CRendererDX();

	bool	Init(HWND hWnd,BOOL bWindow);
	void	Release(void);
	void	Update(void);
	bool	BeginDraw(void);
	void	EndDraw(void);

	//Get関数
	auto	GetDevice(void) { return m_pD3DDevice; }
	auto	GetBGColor(void) const { return m_cBGColor; }

	//Set関数
	void	SetBGColor(const CKFColor& cColor) { m_cBGColor = cColor; }

private:
	//--------------------------------------------------------------------------------
	//  列挙型定義
	//--------------------------------------------------------------------------------
	enum RENDER_MODE
	{
		RM_NORMAL,		//背面カリング、塗りつぶし
		RM_WIREFRAME	//ワイヤーフレーム（デバッグ用）
	};

	//--------------------------------------------------------------------------------
	//  関数定義
	//--------------------------------------------------------------------------------
	void				SetRenderMode(const RENDER_MODE& rm);

	//--------------------------------------------------------------------------------
	//  変数定義
	//--------------------------------------------------------------------------------
	LPDIRECT3D9			m_pD3D;			// Direct3Dオブジェクト
	LPDIRECT3DDEVICE9	m_pD3DDevice;	// Deviceオブジェクト(描画に必要)
	CKFColor			m_cBGColor;		// 背景色
};
#endif