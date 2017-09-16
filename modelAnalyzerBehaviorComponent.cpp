//--------------------------------------------------------------------------------
//	���f���A�i���C�U�[�r�w�C�r�A�R���|�l���g
//�@modelAnalyzerBehaviorComponent.cpp
//	Author : Xu Wenjie
//	Date   : 2017-07-19
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//  �C���N���[�h�t�@�C��
//--------------------------------------------------------------------------------
#include "modelAnalyzerBehaviorComponent.h"
#include "gameObject.h"
#include "transformComponent.h"
#include "KF_Math.h"
#include "KF_UtilityFBX.h"

//--------------------------------------------------------------------------------
//  �N���X
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//  �R���X�g���N�^
//--------------------------------------------------------------------------------
CModelAnalyzerBehaviorComponent::CModelAnalyzerBehaviorComponent(CGameObject* const pGameObj)
	: CBehaviorComponent(pGameObj)
	, m_bDrawNormal(false)
	, m_bEnableCullFace(false)
	, m_bEnableLight(false)
	, m_bReverseV(false)
	, m_bSaved(false)
	, m_pRootNode(nullptr)
{
	m_strFileName.clear();
}

//--------------------------------------------------------------------------------
//  ������
//--------------------------------------------------------------------------------
bool CModelAnalyzerBehaviorComponent::Init(void)
{
	m_strFileName.clear();
	m_bSaved = false;
	m_bReverseV = false;
	auto pTrans = m_pGameObj->GetTransformComponent();
	pTrans->SetPosNext(CKFMath::sc_vZero);
	pTrans->SetForwardNext(CKFMath::sc_vForward);
	pTrans->SetRightNext(CKFMath::sc_vRight);
	pTrans->SetUpNext(CKFMath::sc_vUp);
	pTrans->SetScaleNext(CKFVec3(0.05f)/*CKFMath::sc_vOne*/);
	return true;
}

//--------------------------------------------------------------------------------
//  �I������
//--------------------------------------------------------------------------------
void CModelAnalyzerBehaviorComponent::Uninit(void)
{
	releaseModel();
}

//--------------------------------------------------------------------------------
//  �X�V
//--------------------------------------------------------------------------------
void CModelAnalyzerBehaviorComponent::Update(void)
{
	if (m_pRootNode)
	{
		m_pRootNode->RecursiveUpdate();
	}
}

//--------------------------------------------------------------------------------
//  �X�V
//--------------------------------------------------------------------------------
void CModelAnalyzerBehaviorComponent::LateUpdate(void)
{

}

//--------------------------------------------------------------------------------
// ChangeModel
//--------------------------------------------------------------------------------
void CModelAnalyzerBehaviorComponent::ChangeModel(const string& strFilePath)
{
	//Check File
	string strName, strType;
	CKFUtility::AnalyzeTexPath(strFilePath, strName, strType);
	if (strType._Equal("FBX")
		|| strType._Equal("fbx")
		|| strType._Equal("obj")
		|| strType._Equal("OBJ"))
	{
		//�t���X�N���[�����[�h�m�F
		auto nID = MessageBox(NULL, "���f����؂�ւ��܂����H", "�m�F", MB_YESNO | MB_ICONWARNING | MB_DEFBUTTON2);

		//��������
		if (nID == IDNO) { return; }

		//�O�̃��f���̍폜
		releaseModel();

		//LoadModel
		m_strFileName = strName;
		m_pRootNode = CKFUtilityFBX::Load(strFilePath);
		m_pRootNode->RecursiveRecalculateVtx();
	}
	else
	{//�Ή����ĂȂ�
		MessageBox(NULL, strFilePath.c_str(), "�Ή��ł��Ȃ��^�C�v", MB_OK);
	}
}

//--------------------------------------------------------------------------------
// releaseModel
//--------------------------------------------------------------------------------
void CModelAnalyzerBehaviorComponent::releaseModel(void)
{
	if (!m_pRootNode) { return; }

	if (!m_bSaved)
	{
		//�t���X�N���[�����[�h�m�F
		auto nID = MessageBox(NULL, "�Z�[�u���܂����H", "�m�F", MB_YESNO | MB_ICONWARNING | MB_DEFBUTTON2);

		//��������
		if (nID == IDYES) { /*saveModelAs(m_strFileName);*/ }
	}

	SAFE_RELEASE(m_pRootNode);
	Init();
}