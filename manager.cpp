//--------------------------------------------------------------------------------
//
//�@manager.cpp
//	Author : Xu Wenjie
//	Date   : 2016-11-22
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//  �C���N���[�h�t�@�C��
//--------------------------------------------------------------------------------
#include "main.h"
#include "manager.h"
#include "inputManager.h"
#include "meshManager.h"
#include "textureManager.h"
#include "lightManager.h"
#include "materialManager.h"
#include "gameObjectManager.h"
#include "rendererDX.h"
#include "mode.h"
#include "modeModelAnalyzer.h"
#include "fade.h"

//�������Z����
#include "KF_CollisionSystem.h"
#include "KF_PhysicsSystem.h"

//ImGui
#include "ImGui\imgui.h"
#include "ImGui\imgui_impl_dx9.h"

//--------------------------------------------------------------------------------
//  �ÓI�����o�[�ϐ��錾
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//  �N���X
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//  �R���X�g���N�^
//--------------------------------------------------------------------------------
CManager::CManager()
	: m_pRenderer(nullptr)
	, m_pInputManager(nullptr)
	, m_pMeshManager(nullptr)
	, m_pTextureManager(nullptr)
	, m_pLightManager(nullptr)
	, m_pMaterialManager(nullptr)
	, m_pGameObjectManager(nullptr)
	, m_pMode(nullptr)
	, m_pFade(nullptr)
	, m_pCollisionSystem(nullptr)
	, m_pPhysicsSystem(nullptr)
{
}

//--------------------------------------------------------------------------------
//  ������
//--------------------------------------------------------------------------------
bool CManager::Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow)
{
	HRESULT hr = E_FAIL;

	//�����_��
	CKFMath::InitRandom();

	//�����_���[�̐���
	m_pRenderer = new CRendererDX;

	hr = m_pRenderer->Init(hWnd, bWindow);
	if (FAILED(hr))
	{
		MessageBox(NULL, "m_pRenderer->Init ERROR!!", "�G���[", MB_OK | MB_ICONWARNING);
		return false;
	}

	// Setup ImGui binding
	ImGui_ImplDX9_Init(hWnd, m_pRenderer->GetDevice());

	//���͂̐���
	m_pInputManager = new CInputManager;
	if (!m_pInputManager->Init(hInstance, hWnd))
	{
		MessageBox(NULL, "m_pInputManager->Init ERROR!!", "�G���[", MB_OK | MB_ICONWARNING);
		return false;
	}

	//���b�V���}�l�[�W���̐���
	m_pMeshManager = new CMeshManager;

	//�e�N�X�`���}�l�[�W���̐���
	m_pTextureManager = new CTextureManager;

	//���C�g�}�l�[�W���̐���
	m_pLightManager = new CLightManager;
	if (!m_pLightManager->Init())
	{
		MessageBox(NULL, "m_pLightManager->Init ERROR!!", "�G���[", MB_OK | MB_ICONWARNING);
		return false;
	}

	//�}�e���A���}�l�[�W���̐���
	m_pMaterialManager = new CMaterialManager;

	//�R���W�����V�X�e��
	m_pCollisionSystem = new CKFCollisionSystem;
	m_pCollisionSystem->Init();

	//�������Z�V�X�e��
	m_pPhysicsSystem = new CKFPhysicsSystem;
	if (!m_pPhysicsSystem->Init())
	{
		MessageBox(NULL, "m_pPhysicsSystem->Init ERROR!!", "�G���[", MB_OK | MB_ICONWARNING);
		return false;
	}

	//�Q�[���I�u�W�F�N�g�}�l�[�W���̐���
	m_pGameObjectManager = CGameObjectManager::Create();

	//Fade�̐���
	m_pFade = CFade::Create();

	//�������[�h�ݒ�
	SetMode(new CModeModelAnalyzer);

	return true;
}

//--------------------------------------------------------------------------------
//  �I������
//--------------------------------------------------------------------------------
void CManager::Uninit(void)
{
	//���[�h�̔j��
	SAFE_RELEASE(m_pMode);

	//Fade�̔j��
	SAFE_RELEASE(m_pFade);

	//�Q�[���I�u�W�F�N�g�}�l�[�W���̔j��
	SAFE_RELEASE(m_pGameObjectManager);

	//�������Z�V�X�e���̔j��
	SAFE_RELEASE(m_pPhysicsSystem);

	//�R���W�����V�X�e���̔j��
	SAFE_RELEASE(m_pCollisionSystem);

	//�}�e���A���}�l�[�W���̔j��
	if (m_pMaterialManager)
	{
		delete m_pMaterialManager;
		m_pMaterialManager = nullptr;
	}

	//���C�g�}�l�[�W���̔j��
	if (m_pLightManager)
	{
		m_pLightManager->Uninit();
		delete m_pLightManager;
		m_pLightManager = nullptr;
	}

	//�e�N�X�`���}�l�[�W���̔j��
	SAFE_RELEASE(m_pTextureManager);

	//���b�V���}�l�[�W���̔j��
	SAFE_RELEASE(m_pMeshManager);

	//���̓}�l�[�W���̔j��
	SAFE_RELEASE(m_pInputManager);

	//ImGui
	ImGui_ImplDX9_Shutdown();

	//�����_���[�̔j��
	SAFE_RELEASE(m_pRenderer);
}

//--------------------------------------------------------------------------------
//  �X�V����
//--------------------------------------------------------------------------------
void CManager::Update(void)
{
	//ImGui
	ImGui_ImplDX9_NewFrame();

	//���͍X�V
	m_pInputManager->Update();

	//�����_���[�X�V
	m_pRenderer->Update();

	//���[�h�X�V
	m_pMode->Update();

	//�R���W�����X�V
	m_pCollisionSystem->Update();

	//�������Z�X�V
	m_pPhysicsSystem->Update();
}

//--------------------------------------------------------------------------------
//  �X�V����(�`�撼�O)
//--------------------------------------------------------------------------------
void CManager::LateUpdate(void)
{
	//���[�h�X�V
	m_pMode->LateUpdate();

	//�R���W�����X�V
	m_pCollisionSystem->LateUpdate();

	//Fade�X�V
	m_pFade->Update();
}

//--------------------------------------------------------------------------------
//  �`�揈��
//--------------------------------------------------------------------------------
void CManager::Draw(void)
{
	if (m_pRenderer->BeginDraw())
	{
		//���[�h�`��
		m_pMode->Draw();

#ifdef _DEBUG
		m_pCollisionSystem->DrawCollider();
#endif

		//Fade�`��
		m_pFade->Draw();

		//ImGui
		ImGui::Render();

		m_pRenderer->EndDraw();
	}
}

//--------------------------------------------------------------------------------
//  ���[�h�؂�ւ�
//--------------------------------------------------------------------------------
void CManager::SetMode(CMode* pMode)
{
	if (m_pMode)
	{
		m_pMode->Release();
	}

	m_pMode = pMode;

	if (m_pMode)
	{
		m_pMode->Init();
	}
}