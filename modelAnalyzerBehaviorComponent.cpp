//--------------------------------------------------------------------------------
//	モデルアナライザービヘイビアコンポネント
//　modelAnalyzerBehaviorComponent.cpp
//	Author : Xu Wenjie
//	Date   : 2017-07-19
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//  インクルードファイル
//--------------------------------------------------------------------------------
#include "modelAnalyzerBehaviorComponent.h"
#include "gameObject.h"
#include "transformComponent.h"
#include "KF_Math.h"
#include "drawComponent.h"
#include "materialManager.h"
#include "textureManager.h"
#include "ImGui\imgui.h"
#include "main.h"
#include "manager.h"
#include "rendererDX.h"
#include "mode.h"
#include "camera.h"

//--------------------------------------------------------------------------------
//  クラス
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//
//  Public
//
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//  コンストラクタ
//--------------------------------------------------------------------------------
CModelAnalyzerBehaviorComponent::CModelAnalyzerBehaviorComponent(CGameObject* const pGameObj)
	: CBehaviorComponent(pGameObj)
	, m_bDrawNormal(false)
	, m_bReverseV(false)
	, m_bSaved(false)
	, m_pRootNode(nullptr)
	, m_pAnimator(nullptr)
	, m_bModelInfoWindow(false)
	, m_bAnimatorWindow(false)
	, m_bCameraWindow(false)
	, m_pNodeNow(nullptr)
	, m_vNodeNowCorrectTrans(CKFMath::sc_vZero)
	, m_vNodeNowCorrectRot(CKFMath::sc_vZero)
	, m_vNodeNowCorrectScale(CKFMath::sc_vOne)
	, m_bPlayMotion(false)
	, m_nCntFrame(0)
	, m_nNoMotion(0)
{
	m_strFileName.clear();
}

//--------------------------------------------------------------------------------
//  初期化
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
	pTrans->SetScaleNext(CKFMath::sc_vOne);

	//Collider Material
	CMain::GetManager()->GetMaterialManager()->SaveMaterial(CKFMath::sc_cBlue, CKFMath::sc_cBlue, CKFMath::sc_cBlue, CKFMath::sc_cBlue, 1.0f);
	CMain::GetManager()->GetMaterialManager()->SaveMaterial(CKFMath::sc_cRed, CKFMath::sc_cRed, CKFMath::sc_cRed, CKFMath::sc_cRed, 1.0f);
	return true;
}

//--------------------------------------------------------------------------------
//  終了処理
//--------------------------------------------------------------------------------
void CModelAnalyzerBehaviorComponent::Uninit(void)
{
	releaseModel();
}

//--------------------------------------------------------------------------------
//  更新
//--------------------------------------------------------------------------------
void CModelAnalyzerBehaviorComponent::Update(void)
{
	if (m_pRootNode && m_pAnimator && m_bPlayMotion)
	{
		auto& motion = m_pAnimator->Motions[m_nNoMotion];
		auto& avatar = motion.Frames[m_nCntFrame];
		m_pRootNode->RecursiveUpdate(avatar);
		m_nCntFrame = (m_nCntFrame - motion.StartFrame + 1) % (motion.EndFrame - motion.StartFrame) + motion.StartFrame;
	}
}

//--------------------------------------------------------------------------------
//  更新
//--------------------------------------------------------------------------------
void CModelAnalyzerBehaviorComponent::LateUpdate(void)
{
	// Menu
	showMainMenuBar();

	// Main Window
	showMainWindow();

	// Model Info Window
	showModelInfoWindow();

	// Edit Node Now
	showNodeNowWindow();

	// Animator
	showAnimatorWindow();

	// Camera
	showCameraWindow();
}

//--------------------------------------------------------------------------------
// ChangeModel
//--------------------------------------------------------------------------------
void CModelAnalyzerBehaviorComponent::ChangeModel(const string& strFilePath)
{
	//Check File
	string strName, strType;
	CKFUtility::AnalyzeFilePath(strFilePath, strName, strType);
	if (strType._Equal("FBX")
		|| strType._Equal("fbx")
		|| strType._Equal("obj")
		|| strType._Equal("OBJ")
		|| strType._Equal("txt"))
	{
		if (m_pRootNode)
		{
			auto nID = MessageBox(NULL, "モデルを切り替えますか？", "確認", MB_YESNO | MB_ICONWARNING | MB_DEFBUTTON2);
			if (nID == IDNO) { return; }

			//前のモデルの削除
			releaseModel();
		}

		//ImGui Flag
		m_bModelInfoWindow = false;
		m_bAnimatorWindow = false;
		m_bCameraWindow = false;
		m_nCntFrame = 0;
		m_nNoMotion = 0;

		//LoadModel
		m_strFileName = strName;
		MyModel myModel;
		if (strType._Equal("txt"))
		{
			myModel = CKFUtilityFBX::LoadFromTxt(strFilePath);
		}
		else
		{
			myModel = CKFUtilityFBX::Load(strFilePath);
		}
		m_pRootNode = myModel.pNode;
		m_pAnimator = myModel.pAnimator;
		m_pRootNode->RecursiveRecalculateVtx();
		if (m_pAnimator)
		{
			m_pRootNode->RecursiveRecalculateClusterID(myModel.pAnimator->Motions[0].Frames[0]);
		}
	}
	else
	{//対応してない
		MessageBox(NULL, strFilePath.c_str(), "対応できないタイプ", MB_OK);
	}
}

//--------------------------------------------------------------------------------
// ChangeModel
//--------------------------------------------------------------------------------
void CModelAnalyzerBehaviorComponent::SaveModel(void)
{
	if (CKFUtilityFBX::Save(m_pRootNode, m_strFileName))
	{
		m_bSaved = true;
		MessageBox(NULL, "セーブしました。", "SaveModel", MB_OK);
	}
}

//--------------------------------------------------------------------------------
//
//  Private
//
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
// releaseModel
//--------------------------------------------------------------------------------
void CModelAnalyzerBehaviorComponent::releaseModel(void)
{
	if (!m_bSaved)
	{//Save確認
		auto nID = MessageBox(NULL, "今のモデルセーブしますか？", "確認", MB_YESNO | MB_ICONWARNING | MB_DEFBUTTON2);
		if (nID == IDYES) { SaveModel(); }
	}

	SAFE_RELEASE(m_pRootNode);
	SAFE_RELEASE(m_pAnimator);
	Init();
}

//--------------------------------------------------------------------------------
// mainWindow
//--------------------------------------------------------------------------------
void CModelAnalyzerBehaviorComponent::showMainWindow(void)
{
	auto pRenderer = CMain::GetManager()->GetRenderer();
	auto cBGColor = pRenderer->GetBGColor();

	// Begin
	if (!ImGui::Begin("Model Analyzer Main Window"))
	{
		ImGui::End();
		return;
	}

	// FPS
	ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

	// BG Color
	ImGui::ColorEdit3("BG Color", (float*)&cBGColor);
	pRenderer->SetBGColor(cBGColor);

	// Model Window
	if (ImGui::Button("Model Info")) m_bModelInfoWindow ^= 1;
	
	// Animator Window
	if (m_pAnimator)
	{
		if (ImGui::Button("Animator")) m_bAnimatorWindow ^= 1;
	}

	// Camera Window
	if (ImGui::Button("Camera")) m_bCameraWindow ^= 1;

	// End
	ImGui::End();
}

//--------------------------------------------------------------------------------
// showMainWindowMenuBar
//--------------------------------------------------------------------------------
void CModelAnalyzerBehaviorComponent::showMainMenuBar(void)
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			showMainMenuFile();
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
}

//--------------------------------------------------------------------------------
// showMainWindowFileMenu
//--------------------------------------------------------------------------------
void CModelAnalyzerBehaviorComponent::showMainMenuFile(void)
{
	if (ImGui::MenuItem("Open Model File")) 
	{
		string strFileName;
		if (CMain::OpenModelFile(strFileName))
		{
			ChangeModel(strFileName);
		}
	}
	if (ImGui::MenuItem("Save Model")) 
	{
		SaveModel();
	}
}

//--------------------------------------------------------------------------------
// modelInfoWindow
//--------------------------------------------------------------------------------
void CModelAnalyzerBehaviorComponent::showModelInfoWindow(void)
{
	if (!m_bModelInfoWindow) { return; }

	// Begin
	if (!ImGui::Begin("Model Info Window", &m_bModelInfoWindow))
	{
		ImGui::End();
		return;
	}

	// Model Name
	string strBuf = "Model Name : " + m_strFileName;
	ImGui::Text(strBuf.c_str());

	// Reverse Tex V
	if (ImGui::Checkbox("Reverse Texture V", &m_bReverseV))
	{
		if (m_pRootNode) { m_pRootNode->RecursiveReverseTexV(); }
	}

	// Node Info
	if (ImGui::CollapsingHeader("Node Info"))
	{
		showNodeInfo(m_pRootNode);
	}

	// End
	ImGui::End();
}

//--------------------------------------------------------------------------------
// showNodeInfo
//--------------------------------------------------------------------------------
void CModelAnalyzerBehaviorComponent::showNodeInfo(CMyNode* pNode)
{
	if (!pNode) return;

	if (ImGui::TreeNode(pNode->Name.c_str()))
	{
		if (ImGui::CollapsingHeader("Info"))
		{
			//Type
			for (int nCnt = 0; nCnt < (int)pNode->vecAttributeName.size(); ++nCnt)
			{
				ImGui::Text("Type%d : %s", nCnt, pNode->vecAttributeName[nCnt].c_str());
			}

			//Offset
			ImGui::InputFloat3("Trans", &pNode->vTrans.m_fX);
			ImGui::SliderFloat3("Rot", &pNode->vRot.m_fX, 0.0f, KF_PI * 2.0f);
			ImGui::InputFloat3("Scale", &pNode->vScale.m_fX);

			//Mesh
			if (ImGui::TreeNode("Mesh"))
			{
				for (int nCnt = 0; nCnt < (int)pNode->vecMesh.size(); ++nCnt)
				{
					auto& mesh = pNode->vecMesh[nCnt];
					auto strMeshName = to_string(nCnt);
					if (ImGui::TreeNode(strMeshName.c_str()))
					{
						//Info
						ImGui::Text("NumPolygon : %d", mesh.m_nNumPolygon);
						ImGui::Text("NumVtx : %d  NumIdx : %d", mesh.m_nNumVtx, mesh.m_nNumIdx);

						// Light
						ImGui::Checkbox("Enable Light", &mesh.m_bEnableLight);

						// CullFace
						ImGui::Checkbox("Enable CullFace", &mesh.m_bEnableCullFace);

						// Texture
						ImGui::Text("Texture : %s", mesh.strTexName.c_str());
						if (ImGui::Button("Change Texture"))
						{
							changeTexture(mesh);
						}

						// Render Priority
						static const char* listbox_rp[] =
						{ "3D"
							, "3D_ALPHATEST"
							, "3D_ZSORT" };
						ImGui::ListBox("Render Priority\n(single select)", (int*)&mesh.m_renderPriority, listbox_rp, 3, 3);
						ImGui::TreePop();
					}
				}
				ImGui::TreePop();
			}

			//Collider
			ImGui::Text("Collider : %d", (int)pNode->listCollider.size());

			//Edit
			if (!m_pNodeNow && ImGui::Button("Edit Node"))
			{
				m_pNodeNow = pNode;
				m_pNodeNow->materialID = 2;
			}
		}

		if (ImGui::CollapsingHeader("Child"))
		{
			//Child
			for (auto pChild : pNode->listChild)
			{
				showNodeInfo(pChild);
			}
		}

		ImGui::TreePop();
	}
}

//--------------------------------------------------------------------------------
// showNodeInfo
//--------------------------------------------------------------------------------
void CModelAnalyzerBehaviorComponent::showNodeNowWindow(void)
{
	if (!m_pNodeNow) { return; }
	bool bNodeNow = true;

	// Begin
	if (!ImGui::Begin("Edit Node Now Window", &bNodeNow))
	{
		ImGui::End();
		return;
	}

	// Model Name
	string strBuf = "Node Name : " + m_pNodeNow->Name;
	ImGui::Text(strBuf.c_str());

	// Collider
	if (ImGui::CollapsingHeader("Collider"))
	{
		//Edit Collider
		static const char* listbox_items[] =
		{ "Sphere"
			, "AABB"
			, "OBB" };
		int nCnt = 0;
		for (auto itr = m_pNodeNow->listCollider.begin(); itr != m_pNodeNow->listCollider.end();)
		{
			bool bDelete = false;
			char aBuf[128];
			wsprintf(aBuf, "Collider%d", nCnt);
			if (ImGui::TreeNode(aBuf))
			{
				//Type
				ImGui::ListBox("Collider Type\n(single select)", (int*)&itr->colType, listbox_items, 3, 3);
				
				//Offset
				ImGui::InputFloat3("Trans", &itr->vOffsetPos.m_fX);
				ImGui::SliderFloat3("Rot", &itr->vOffsetRot.m_fX, 0.0f, KF_PI * 2.0f);
				ImGui::InputFloat3("Scale", &itr->vOffsetScale.m_fX);

				if (itr->colType == CS::COL_SPHERE)
				{//same scale xyz by x
					itr->vOffsetScale.m_fY =
						itr->vOffsetScale.m_fZ =
						itr->vOffsetScale.m_fX;
				}

				bDelete = ImGui::Button("Delete Collider");
				ImGui::TreePop();
			}

			if(bDelete)
			{
				itr = m_pNodeNow->listCollider.erase(itr);
			}
			else
			{
				++itr;
				++nCnt;
			}
		}

		//Add Collider
		if (ImGui::Button("Add Collider"))
		{
			COL_INFO col;
			col.colType = CS::COL_SPHERE;
			col.vOffsetPos = CKFMath::sc_vZero;
			col.vOffsetRot = CKFMath::sc_vZero;
			col.vOffsetScale = CKFMath::sc_vOne;
			m_pNodeNow->listCollider.push_back(col);
		}
	}

	// Fix Vtx
	if (ImGui::CollapsingHeader("Fix Vtx"))
	{
		//Offset
		ImGui::InputFloat3("Fixed Trans", &m_vNodeNowCorrectTrans.m_fX);
		ImGui::InputFloat3("Fixed Rot", &m_vNodeNowCorrectRot.m_fX);
		ImGui::InputFloat3("Fixed Scale", &m_vNodeNowCorrectScale.m_fX);

		// Recalculate By Mtx
		if (ImGui::Button("Fix Vertex"))
		{
			CKFMtx44 mtxThis;

			//拡縮
			mtxThis.m_af[0][0] = m_vNodeNowCorrectScale.m_fX;
			mtxThis.m_af[1][1] = m_vNodeNowCorrectScale.m_fY;
			mtxThis.m_af[2][2] = m_vNodeNowCorrectScale.m_fZ;

			//回転
			CKFMtx44 mtxRot;
			CKFMath::MtxRotationYawPitchRoll(mtxRot, m_vNodeNowCorrectRot);
			mtxThis *= mtxRot;

			//平行移動
			CKFMtx44 mtxPos;
			CKFMath::MtxTranslation(mtxPos, m_vNodeNowCorrectTrans);
			mtxThis *= mtxPos;

			//Recalculate
			m_pNodeNow->RecalculateVtxByMatrix(mtxThis);

			//Reset
			m_vNodeNowCorrectTrans = CKFMath::sc_vZero;
			m_vNodeNowCorrectRot = CKFMath::sc_vZero;
			m_vNodeNowCorrectScale = CKFMath::sc_vOne;
		}
	}

	// End
	ImGui::End();

	if (!bNodeNow) 
	{
		m_pNodeNow->materialID = 1;
		m_pNodeNow = nullptr;
		m_vNodeNowCorrectTrans = CKFMath::sc_vZero;
		m_vNodeNowCorrectRot = CKFMath::sc_vZero;
		m_vNodeNowCorrectScale = CKFMath::sc_vOne;
	}
}

//--------------------------------------------------------------------------------
// showAnimatorWindow
//--------------------------------------------------------------------------------
void CModelAnalyzerBehaviorComponent::showAnimatorWindow(void)
{
	if (!m_bAnimatorWindow) return;

	// Play
	if (ImGui::Button("Play")) m_bPlayMotion ^= 1;
	
	// new
	int nNumMotion = (int)m_pAnimator->Motions.size();
	char **arr = new char*[nNumMotion];
	for (int nCnt = 0; nCnt < nNumMotion; ++nCnt)
	{
		auto& strName = m_pAnimator->Motions[nCnt].Name;
		int nNumChar = (int)strName.size();
		arr[nCnt] = new char[nNumChar + 1];
		for (int nCntChar = 0; nCntChar < nNumChar; ++nCntChar)
		{
			arr[nCnt][nCntChar] = strName[nCntChar];
		}
		arr[nCnt][nNumChar] = '\0';
	}

	//Type
	if (ImGui::ListBox("Select Motion\n(single select)", (int*)&m_nNoMotion, arr, nNumMotion, nNumMotion))
	{
		m_nCntFrame = 0;
	}

	//delete
	for (int nCnt = 0; nCnt < nNumMotion; ++nCnt)
	{
		delete[] arr[nCnt];
		arr[nCnt] = nullptr;
	}
	delete[] arr;
	arr = nullptr;

	// Animation Current
	auto& current = m_pAnimator->Motions[m_nNoMotion];
	ImGui::Text("CurrentAnimation : %s", current.Name.c_str());
	char buffer[256] = {};
	if (ImGui::InputText("EditName", buffer, 256))
	{
		current.Name = buffer;
	}

	// Start Frame
	int startFrame = current.StartFrame;
	if (ImGui::InputInt("StartFrame", &startFrame))
	{
		if (startFrame <= current.EndFrame)
		{
			current.StartFrame = startFrame;
		}
	}

	// End Frame
	int endFrame = current.EndFrame;
	if (ImGui::InputInt("EndFrame", &endFrame))
	{
		if (endFrame >= current.StartFrame)
		{
			current.EndFrame = endFrame;
		}
	}

	// Delete Frame that out of range
	if (ImGui::Button("Delete frames that out of range"))
	{
		if (current.StartFrame > 0 || current.EndFrame < current.Frames.size() - 1)
		{
			vector<Frame> newFrames;
			newFrames.reserve(current.EndFrame - current.StartFrame);
			for (int count = current.StartFrame; count < current.EndFrame; ++count)
			{
				newFrames.push_back(current.Frames[count]);
			}
			current.Frames.clear();
			for (auto& frame : newFrames)
			{
				current.Frames.push_back(frame);
			}
			current.Frames.shrink_to_fit();
			current.StartFrame = 0;
			current.EndFrame = current.Frames.size() - 1;
		}
	}

	// AddAnimation
	if (ImGui::Button("Add Animation"))
	{
		addAnimation();
	}
}

//--------------------------------------------------------------------------------
// showCameraWindow
//--------------------------------------------------------------------------------
void CModelAnalyzerBehaviorComponent::showCameraWindow(void)
{
	if (!m_bCameraWindow) { return; }

	// Begin
	if (!ImGui::Begin("Camera Window", &m_bCameraWindow))
	{
		ImGui::End();
		return;
	}

	auto pCamera = CMain::GetManager()->GetMode()->GetCamera();
	auto vPosAt = pCamera->GetPosAt();
	auto vPosEye = pCamera->GetPosEye();

	// PosAt
	if (ImGui::InputFloat3("PosAt", &vPosAt.m_fX))
	{
		pCamera->SetPosAt(vPosAt);
	}
	
	// PosEye
	if (ImGui::InputFloat3("PosEye", &vPosEye.m_fX))
	{
		pCamera->SetPosEye(vPosEye);
	}

	// End
	ImGui::End();
}

//--------------------------------------------------------------------------------
// changeTexture
//--------------------------------------------------------------------------------
void CModelAnalyzerBehaviorComponent::changeTexture(Mesh& mesh)
{
	string strTex;
	if (CMain::OpenTextureFile(strTex))
	{
		string strName, strType;
		CKFUtility::AnalyzeFilePath(strTex, strName, strType);
		if (CKFUtility::CheckIsTexture(strType))
		{
			if (!mesh.strTexName.empty())
			{
				CMain::GetManager()->GetTextureManager()->DisuseTexture(mesh.strTexName);
			}
			mesh.strTexName = strName + '.' + strType;
			CMain::GetManager()->GetTextureManager()->UseTexture(mesh.strTexName);
		}
	}
}

//--------------------------------------------------------------------------------
// addAnimation
//--------------------------------------------------------------------------------
void CModelAnalyzerBehaviorComponent::addAnimation(void)
{
	string strFileName;
	if (CMain::OpenTextureFile(strFileName))
	{
		string strName, strType;
		CKFUtility::AnalyzeFilePath(strFileName, strName, strType);
		if (strType._Equal("fbx") || strType._Equal("FBX"))
		{
			CKFUtilityFBX::LoadAnimation(strFileName, m_pAnimator);
		}
	}
}