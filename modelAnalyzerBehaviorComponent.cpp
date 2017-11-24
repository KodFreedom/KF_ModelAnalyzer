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
#include "inputManager.h"
#include "inputDX.h"

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
	, m_bDrawSkeleton(true)
	, m_bDrawMesh(true)
	, m_bDrawCollider(true)
	, m_bReverseV(false)
	, m_bSaved(false)
	, m_pRootNode(nullptr)
	, m_pAnimator(nullptr)
	, m_bModelInfoWindow(false)
	, m_bAnimatorWindow(false)
	, m_bCameraWindow(false)
	, m_bMaterialWindow(false)
	, m_pNodeNow(nullptr)
	, m_vNodeNowCorrectTrans(CKFMath::sc_vZero)
	, m_vNodeNowCorrectRot(CKFMath::sc_vZero)
	, m_vNodeNowCorrectScale(CKFMath::sc_vOne)
	, m_bPlayMotion(false)
	, m_nCntFrame(0)
	, m_nNoMotion(0)
	, m_fRotSpeed(0.005f)
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
	m_bModelInfoWindow = false;
	m_bAnimatorWindow = false;
	m_bCameraWindow = false;
	m_bMaterialWindow = false;
	m_bPlayMotion = false;
	m_nCntFrame = 0;
	m_nNoMotion = 0;
	m_vNodeNowCorrectTrans = CKFMath::sc_vZero;
	m_vNodeNowCorrectRot = CKFMath::sc_vZero;
	m_vNodeNowCorrectScale = CKFMath::sc_vOne;
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
	if (!m_pRootNode) return;

	if (m_pAnimator && m_bPlayMotion)
	{
		auto& motion = m_pAnimator->Motions[m_nNoMotion];
		auto& avatar = motion.Frames[m_nCntFrame];
		m_pAnimator->UpdateBones(avatar);
		m_nCntFrame = (m_nCntFrame - motion.StartFrame + 1) % (motion.EndFrame - motion.StartFrame) + motion.StartFrame;
	}

	m_pRootNode->RecursiveUpdateMatrix(m_pGameObj->GetTransformComponent()->GetMatrix());

	if (m_pAnimator)
	{
		m_pAnimator->UpdateClusterWorld();
		m_pRootNode->RecursiveUpdateSkin(m_pAnimator->Clusters);
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

	// Material
	showMaterialWindow();

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
		m_mapMaterial = myModel.mapMaterial;
		for (auto& pair : m_mapMaterial)
		{
			CMain::GetManager()->GetTextureManager()->UseTexture(pair.second.DiffuseTextureName);
		}
		m_pRootNode->RecursiveRecombineMeshes();
		if (m_pAnimator)
		{
			m_pRootNode->RecursiveMatchClusterID(myModel.pAnimator->Motions[0].Frames[0]);
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
void CModelAnalyzerBehaviorComponent::SaveModel(const OutType& type)
{
	MyModel model;
	model.pAnimator = m_pAnimator;
	model.pNode = m_pRootNode;
	model.mapMaterial = m_mapMaterial;
	if (CKFUtilityFBX::Save(model, m_strFileName, type))
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
	if (!m_bSaved && m_pRootNode)
	{//Save確認
		auto nID = MessageBox(NULL, "今のモデルセーブしますか？", "確認", MB_YESNO | MB_ICONWARNING | MB_DEFBUTTON2);
		if (nID == IDYES) 
		{
			auto nID = MessageBox(NULL, "Jsonでセーブしますか？(Noの場合Binaryでセーブします)", "確認", MB_YESNO | MB_ICONWARNING | MB_DEFBUTTON2);
			SaveModel(nID == IDYES ? Json : Binary);
		}
	}
	SAFE_RELEASE(m_pRootNode);
	SAFE_RELEASE(m_pAnimator);
	for (auto& pair : m_mapMaterial)
	{
		CMain::GetManager()->GetTextureManager()->DisuseTexture(pair.second.DiffuseTextureName);
	}
	m_mapMaterial.clear();
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
	if (!ImGui::Begin("MainWindow"))
	{
		ImGui::End();
		return;
	}

	// FPS
	ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

	// BG Color
	ImGui::ColorEdit3("Background color", (float*)&cBGColor);
	pRenderer->SetBGColor(cBGColor);

	// Model Window
	if (m_pRootNode)
	{
		if (ImGui::Button(m_bModelInfoWindow ? 
			"Close model window" :
			"Open model window")) m_bModelInfoWindow ^= 1;

		// Animator Window
		if (m_pAnimator)
		{
			if (ImGui::Button(m_bAnimatorWindow ?
				"Close animator window" :
				"Open animator window")) m_bAnimatorWindow ^= 1;
		}

		// Material Window
		if (ImGui::Button(m_bMaterialWindow ?
			"Close material window" :
			"Open material window")) m_bMaterialWindow ^= 1;
	}

	// Camera Window
	if (ImGui::Button(m_bCameraWindow ?
		"Close camera window" :
		"Open camera window")) m_bCameraWindow ^= 1;

	// DrawFlag
	if (ImGui::Button(m_bDrawSkeleton ? 
		"Disdraw skeleton" : "Draw skeleton")) m_bDrawSkeleton ^= 1;
	if (ImGui::Button(m_bDrawMesh ?
		"Disdraw mesh" : "Draw mesh")) m_bDrawMesh ^= 1;
	if (ImGui::Button(m_bDrawCollider ?
		"Disdraw collider" : "Draw collider")) m_bDrawCollider ^= 1;

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
	if (ImGui::MenuItem("Open model file")) 
	{
		string strFileName;
		if (CMain::OpenModelFile(strFileName))
		{
			ChangeModel(strFileName);
		}
	}
	if (ImGui::MenuItem("Save as Json")) 
	{
		SaveModel(Json);
	}
	if (ImGui::MenuItem("Save as Binary"))
	{
		SaveModel(Binary);
	}
}

//--------------------------------------------------------------------------------
// modelInfoWindow
//--------------------------------------------------------------------------------
void CModelAnalyzerBehaviorComponent::showModelInfoWindow(void)
{
	if (!m_bModelInfoWindow) return;

	// Begin
	if (!ImGui::Begin("ModelInfoWindow", &m_bModelInfoWindow))
	{
		ImGui::End();
		return;
	}

	// Model Name
	char buffer[bufferSize] = {};
	strcpy_s(buffer, m_strFileName.c_str());
	if (ImGui::InputText("Name", buffer, bufferSize))
	{
		m_strFileName = buffer;
	}

	// RotSpeed
	ImGui::InputFloat("RotationSpeed", &m_fRotSpeed);

	// Reverse Tex V
	if (ImGui::Checkbox("Reverse TextureV", &m_bReverseV))
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
			int size = (int)pNode->AttributeNames.size();
			for (int nCnt = 0; nCnt < size; ++nCnt)
			{
				ImGui::Text("Type%d : %s", nCnt, pNode->AttributeNames[nCnt].c_str());
			}

			//Offset
			ImGui::InputFloat3("Transform", &pNode->Translation.m_fX);
			ImGui::DragFloat3("Rotation", &pNode->RotationOffset.m_fX, m_fRotSpeed, 0.0f, KF_PI * 2.0f);
			ImGui::InputFloat3("Scaling", &pNode->Scale.m_fX);

			//Mesh
			if (!pNode->Meshes.empty() && ImGui::TreeNode("Mesh"))
			{
				for (int nCnt = 0; nCnt < (int)pNode->Meshes.size(); ++nCnt)
				{
					auto& mesh = pNode->Meshes[nCnt];
					auto& strMeshName = to_string(nCnt);
					if (ImGui::TreeNode(strMeshName.c_str()))
					{
						//Info
						ImGui::Text("PolygonNumber : %d", mesh.PolygonNumber);
						ImGui::Text("VertexNumber : %d  IndexNumber : %d", mesh.VertexNumber, mesh.IndexNumber);

						//Material
						char buffer[bufferSize] = {};
						strcpy_s(buffer, mesh.MaterialName.c_str());
						if (ImGui::InputText("MaterialName", buffer, bufferSize))
						{
							mesh.MaterialName = buffer;
						}

						// Render Priority
						static const char* listbox_rp[] =
						{ "3D"
							, "3D_ALPHATEST"
							, "3D_ZSORT" };
						ImGui::ListBox("Render Priority\n(single select)", (int*)&mesh.MyRenderPriority, listbox_rp, 3, 3);

						// Light
						ImGui::Checkbox("Enable Light", &mesh.EnableLight);

						// CullFace
						ImGui::Checkbox("Enable CullFace", &mesh.EnableCullFace);

						// Fog
						ImGui::Checkbox("Enable Fog", &mesh.EnableFog);
						ImGui::TreePop();
					}
				}
				ImGui::TreePop();
			}

			//Collider
			if (!pNode->Colliders.empty() && ImGui::TreeNode("Collider"))
			{
				ImGui::Text("Collider : %d", (int)pNode->Colliders.size());
				ImGui::TreePop();
			}

			//Edit
			if (!m_pNodeNow && ImGui::Button("Edit Node"))
			{
				m_pNodeNow = pNode;
				m_pNodeNow->ColliderMaterialID = 2;
			}
		}

		if (!pNode->Children.empty() && ImGui::CollapsingHeader("Child"))
		{
			//Child
			for (auto pChild : pNode->Children)
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
		for (auto itr = m_pNodeNow->Colliders.begin(); itr != m_pNodeNow->Colliders.end();)
		{
			bool bDelete = false;
			char aBuf[128];
			wsprintf(aBuf, "Collider%d", nCnt);
			if (ImGui::TreeNode(aBuf))
			{
				//Type
				ImGui::ListBox("Collider Type\n(single select)", (int*)&itr->Type, listbox_items, 3, 3);
				
				//Offset
				ImGui::InputFloat3("Translation", &itr->Position.m_fX);
				ImGui::DragFloat3("Rotation", &itr->Rotation.m_fX, m_fRotSpeed, 0.0f, KF_PI * 2.0f);
				ImGui::InputFloat3("Scale", &itr->Scale.m_fX);

				if (itr->Type == CS::COL_SPHERE)
				{//same scale xyz by x
					itr->Scale.m_fY =
						itr->Scale.m_fZ =
						itr->Scale.m_fX;
				}

				bDelete = ImGui::Button("Delete Collider");
				ImGui::TreePop();
			}

			if(bDelete)
			{
				itr = m_pNodeNow->Colliders.erase(itr);
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
			ColliderInfo col;
			col.Type = CS::COL_SPHERE;
			col.Position = CKFMath::sc_vZero;
			col.Rotation = CKFMath::sc_vZero;
			col.Scale = CKFMath::sc_vOne;
			m_pNodeNow->Colliders.push_back(col);
		}
	}

	// Fix Vtx
	if (ImGui::CollapsingHeader("Fix Verteces"))
	{
		//Offset
		ImGui::InputFloat3("Fixed Trans", &m_vNodeNowCorrectTrans.m_fX);
		ImGui::InputFloat3("Fixed Rot", &m_vNodeNowCorrectRot.m_fX);
		ImGui::InputFloat3("Fixed Scale", &m_vNodeNowCorrectScale.m_fX);

		// Recalculate By Mtx
		if (ImGui::Button("Fix Verteces"))
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
			m_pNodeNow->RecalculateMeshesBy(mtxThis);

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
		m_pNodeNow->ColliderMaterialID = 1;
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
	if (!m_bAnimatorWindow || m_pAnimator->Motions.empty()) return;

	// Begin
	if (!ImGui::Begin("Animation Window", &m_bAnimatorWindow))
	{
		ImGui::End();
		return;
	}

	// Play
	if (ImGui::Button(m_bPlayMotion ? "Pause" : "Play")) m_bPlayMotion ^= 1;
	
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
	showCurrentAnimationWindow();

	// AddAnimation
	if (ImGui::Button("Add animation"))
	{
		addAnimation();
	}

	// End
	ImGui::End();
}

//--------------------------------------------------------------------------------
// showMaterialWindow
//--------------------------------------------------------------------------------
void CModelAnalyzerBehaviorComponent::showMaterialWindow(void)
{
	if (!m_bMaterialWindow) { return; }

	// Begin
	if (!ImGui::Begin("Material Window", &m_bMaterialWindow))
	{
		ImGui::End();
		return;
	}

	if (ImGui::CollapsingHeader("Materials"))
	{
		for (auto& pair : m_mapMaterial)
		{
			if (ImGui::TreeNode(pair.first.c_str()))
			{
				ImGui::ColorEdit3("Diffuse", (float*)&pair.second.Diffuse);
				ImGui::ColorEdit3("Ambient", (float*)&pair.second.Ambient);
				ImGui::ColorEdit3("Specular", (float*)&pair.second.Specular);
				ImGui::ColorEdit3("Emissive", (float*)&pair.second.Emissive);
				ImGui::InputFloat("Power", &pair.second.Power);
				ImGui::Text("DiffuseTexture : %s", pair.second.DiffuseTextureName.c_str());
				if (ImGui::Button("Change Diffuse Texture"))
				{
					changeTexture(pair.second.DiffuseTextureName);
				}
				ImGui::Text("SpecularTexture : %s", pair.second.SpecularTextureName.c_str());
				if (ImGui::Button("Change Specular Texture"))
				{
					changeTexture(pair.second.SpecularTextureName);
				}
				ImGui::Text("NormalTexture : %s", pair.second.NormalTextureName.c_str());
				if (ImGui::Button("Change Normal Texture"))
				{
					changeTexture(pair.second.NormalTextureName);
				}
				ImGui::TreePop();
			}
		}
	}
	
	// Add Material
	if (ImGui::CollapsingHeader("Add Material"))
	{
		static string name;
		static Material material;

		char buffer[bufferSize] = {};
		strcpy_s(buffer, name.c_str());
		if (ImGui::InputText("EditName", buffer, bufferSize))
		{
			name = buffer;
		}

		ImGui::ColorEdit3("Diffuse", (float*)&material.Diffuse);
		ImGui::ColorEdit3("Ambient", (float*)&material.Ambient);
		ImGui::ColorEdit3("Specular", (float*)&material.Specular);
		ImGui::ColorEdit3("Emissive", (float*)&material.Emissive);
		ImGui::DragFloat("Power", &material.Power);
		ImGui::Text("DiffuseTexture : %s", material.DiffuseTextureName.c_str());
		if (ImGui::Button("Change Diffuse Texture"))
		{
			changeTexture(material.DiffuseTextureName);
		}
		ImGui::Text("SpecularTexture : %s", material.SpecularTextureName.c_str());
		if (ImGui::Button("Change Specular Texture"))
		{
			changeTexture(material.SpecularTextureName);
		}
		ImGui::Text("NormalTexture : %s", material.NormalTextureName.c_str());
		if (ImGui::Button("Change Normal Texture"))
		{
			changeTexture(material.NormalTextureName);
		}

		if (ImGui::Button("Add to Materials"))
		{
			m_mapMaterial.emplace(name, material);
			if (material.DiffuseTextureName.empty())
			{
				CMain::GetManager()->GetTextureManager()->UseTexture(material.DiffuseTextureName);
			}
			name.clear();
			material.DiffuseTextureName.clear();
			material.NormalTextureName.clear();
			material.SpecularTextureName.clear();
			material.Diffuse = CKFColor(1.0f);
			material.Ambient = CKFColor(0.2f, 0.2f, 0.2f, 1.0f);
			material.Specular = CKFColor(0.0f, 0.0f, 0.0f, 1.0f);
			material.Emissive = CKFColor(0.0f, 0.0f, 0.0f, 1.0f);
			material.Power = 1.0f;
		}
	}

	// End
	ImGui::End();
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
// showCurrentAnimationWindow
//--------------------------------------------------------------------------------
void CModelAnalyzerBehaviorComponent::showCurrentAnimationWindow(void)
{
	if (ImGui::CollapsingHeader("CurrentAnimation"))
	{
		auto& current = m_pAnimator->Motions[m_nNoMotion];
		
		// 名前
		char buffer[bufferSize] = {};
		strcpy_s(buffer, current.Name.c_str());
		if (ImGui::InputText("Name", buffer, bufferSize))
		{
			current.Name = buffer;
		}

		// 今のフレーム
		ImGui::Text("Current Frame : %d", m_nCntFrame);

		// フレーム編集
		if (ImGui::TreeNode("Edit Frame"))
		{
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
			if (ImGui::Button("Delete out of range frames"))
			{
				m_pAnimator->DeleteOutOfRangeFrames(m_nNoMotion);
			}

			ImGui::TreePop();
		}

		// モーション切り替え設定
		if (ImGui::TreeNode("StateTransision"))
		{
			ImGui::Checkbox("IsLoop", &current.IsLoop);

			for (auto iterator = current.Transisions.begin(); iterator != current.Transisions.end();)
			{
				bool isDeleteTransision = false;
				if (ImGui::TreeNode("Transision"))
				{
					// 次のモーション名
					char nameBuffer[bufferSize] = {};
					strcpy_s(nameBuffer, iterator->NextMotion.c_str());
					if (ImGui::InputText("NextAnimationName", nameBuffer, bufferSize))
					{
						iterator->NextMotion = nameBuffer;
					}

					// 条件
					for (auto itrCondition = iterator->Conditions.begin(); itrCondition != iterator->Conditions.end();)
					{
						bool isDeleteCondition = false;
						if (ImGui::TreeNode("Condition"))
						{
							// 条件変数の型
							const char* listboxType[] =
							{
								"bool",
								"float",
							};
							ImGui::ListBox("ParameterType\n(single select)", (int*)&itrCondition->ParameterType, listboxType, 2, 2);

							// 条件変数名
							char parameterNameBuffer[bufferSize] = {};
							strcpy_s(parameterNameBuffer, itrCondition->ParameterName.c_str());
							if (ImGui::InputText("ParameterName", parameterNameBuffer, bufferSize))
							{
								itrCondition->ParameterName = parameterNameBuffer;
							}

							// オペレーター
							if (itrCondition->ParameterType == eParameterType::eFloat)
							{
								const char* listboxOperator[] =
								{
									"equal",
									"notEqual",
									"greater",
									"less"
								};
								ImGui::ListBox("Operator\n(single select)", (int*)&itrCondition->ParameterType, listboxOperator, 4, 4);
							}

							// 値
							switch (itrCondition->ParameterType)
							{
							case eParameterType::eBool:
							{
								const char* listboxValue[] =
								{
									"isFalse",
									"isTrue",
								};
								ImGui::ListBox("Value\n(single select)", (int*)&itrCondition->BoolValue, listboxValue, 2, 2);
								break;
							}
							case eParameterType::eFloat:
							{
								ImGui::InputFloat("Value", &itrCondition->FloatValue);
								break;
							}
							}

							// 条件の削除
							if (ImGui::Button("Delete Condition"))
							{
								isDeleteCondition = true;
							}
							ImGui::TreePop();
						}

						// Count iterator
						if (isDeleteCondition)
						{
							itrCondition = iterator->Conditions.erase(itrCondition);
						}
						else
						{
							++itrCondition;
						}
					}

					// 条件の追加
					if (ImGui::Button("Add Condition"))
					{
						iterator->Conditions.push_back(Condition());
					}

					// 切り替えの削除
					if (ImGui::Button("Delete transision"))
					{
						isDeleteTransision = true;
					}
					ImGui::TreePop();
				}

				// Count iterator
				if (isDeleteTransision)
				{
					iterator = current.Transisions.erase(iterator);
				}
				else
				{
					++iterator;
				}
			}

			// 切り替えの追加
			if (ImGui::Button("Add transision"))
			{
				current.Transisions.push_back(StateTransision());
			}
			ImGui::TreePop();
		}

		// Todo : Delete Animation
		//if (ImGui::Button("Delete animation"))
		//{
		//
		//}
	}
}

//--------------------------------------------------------------------------------
// changeTexture
//--------------------------------------------------------------------------------
void CModelAnalyzerBehaviorComponent::changeTexture(string& meshTexture)
{
	string strTex;
	if (CMain::OpenTextureFile(strTex))
	{
		string strName, strType;
		CKFUtility::AnalyzeFilePath(strTex, strName, strType);
		if (CKFUtility::CheckIsTexture(strType))
		{
			if (!meshTexture.empty())
			{
				CMain::GetManager()->GetTextureManager()->DisuseTexture(meshTexture);
			}
			meshTexture = strName + '.' + strType;
			CMain::GetManager()->GetTextureManager()->UseTexture(meshTexture);
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