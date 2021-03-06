//--------------------------------------------------------------------------------
//  メインマネージャ
//　manager.h
//	Author : Xu Wenjie
//	Date   : 2016-11-22
//--------------------------------------------------------------------------------
#pragma once

//--------------------------------------------------------------------------------
//  インクルードファイル
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//  前方宣言
//--------------------------------------------------------------------------------
class CRendererDX;
class CInputManager;
class CMeshManager;
class CTextureManager;
class CLightManager;
class CMaterialManager;
class CGameObjectManager;
class CMode;
class CFade;
class CKFCollisionSystem;
class CKFPhysicsSystem;

//--------------------------------------------------------------------------------
//  クラス宣言
//--------------------------------------------------------------------------------
class CManager
{
public:
	CManager();
	~CManager() {}

	bool Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow);
	void Uninit(void);
	void Update(void);
	void LateUpdate(void);
	void Draw(void);

	//Set関数
	void SetMode(CMode* pMode);

	//Get関数
	auto GetRenderer(void) { return m_pRenderer; }
	auto GetInputManager(void) { return m_pInputManager; }
	auto GetMeshManager(void) { return m_pMeshManager; }
	auto GetTextureManager(void) { return m_pTextureManager; }
	auto GetLightManager(void) { return m_pLightManager; }
	auto GetMaterialManager(void) { return m_pMaterialManager; }
	auto GetGameObjectManager(void) { return m_pGameObjectManager; }
	auto GetMode(void) { return m_pMode; }
	auto GetFade(void) { return m_pFade; }
	auto GetCollisionSystem(void) { return m_pCollisionSystem; }
	auto GetPhysicsSystem(void) { return m_pPhysicsSystem; }

private:
	//--------------------------------------------------------------------------------
	//  変数定義
	//--------------------------------------------------------------------------------
	CRendererDX*		m_pRenderer;
	CInputManager*		m_pInputManager;
	CMeshManager*		m_pMeshManager;
	CTextureManager*	m_pTextureManager;
	CLightManager*		m_pLightManager;
	CMaterialManager*	m_pMaterialManager;
	CGameObjectManager*	m_pGameObjectManager;
	CMode*				m_pMode;
	CFade*				m_pFade;
	CKFCollisionSystem*	m_pCollisionSystem;
	CKFPhysicsSystem*	m_pPhysicsSystem;
};