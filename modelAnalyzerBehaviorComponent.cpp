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

namespace ImGui
{
	static auto vector_getter = [](void* vec, int idx, const char** out_text)
	{
		auto& vector = *static_cast<std::vector<std::string>*>(vec);
		if (idx < 0 || idx >= static_cast<int>(vector.size())) { return false; }
		*out_text = vector.at(idx).c_str();
		return true;
	};

	bool Combo(const char* label, int* currIndex, std::vector<std::string>& values)
	{
		if (values.empty()) { return false; }
		return Combo(label, currIndex, vector_getter,
			static_cast<void*>(&values), values.size());
	}

	bool ListBox(const char* label, int* currIndex, std::vector<std::string>& values)
	{
		if (values.empty()) { return false; }
		return ListBox(label, currIndex, vector_getter,
			static_cast<void*>(&values), values.size());
	}

}

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
	, is_render_skeletons_(true)
	, is_render_meshes_(true)
	, is_render_colliders_(true)
	, is_reserve_texcoordv_(false)
	, is_saved_(false)
	, root_node_(nullptr)
	, animator_(nullptr)
	, is_display_model_window_(false)
	, is_display_animator_window_(false)
	, is_display_camera_window_(false)
	, is_display_material_window_(false)
	, current_node_(nullptr)
	, current_node_correct_translation_(CKFMath::sc_vZero)
	, current_node_correct_rotation_(CKFMath::sc_vZero)
	, current_node_correct_scale_(CKFMath::sc_vOne)
	, is_playing_motion_(false)
	, current_frame_(0)
	, motion_no_(0)
	, rotation_speed_(0.005f)
{
	file_name_.clear();
}

//--------------------------------------------------------------------------------
//  初期化
//--------------------------------------------------------------------------------
bool CModelAnalyzerBehaviorComponent::Init(void)
{
	file_name_.clear();
	is_saved_ = false;
	is_reserve_texcoordv_ = false;
	is_display_model_window_ = false;
	is_display_animator_window_ = false;
	is_display_camera_window_ = false;
	is_display_material_window_ = false;
	is_playing_motion_ = false;
	current_frame_ = 0;
	motion_no_ = 0;
	current_node_correct_translation_ = CKFMath::sc_vZero;
	current_node_correct_rotation_ = CKFMath::sc_vZero;
	current_node_correct_scale_ = CKFMath::sc_vOne;
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
	ReleaseModel();
}

//--------------------------------------------------------------------------------
//  更新
//--------------------------------------------------------------------------------
void CModelAnalyzerBehaviorComponent::Update(void)
{
	if (!root_node_) return;

	if (animator_ && !animator_->Motions.empty())
	{
		auto& motion = animator_->Motions[motion_no_];

		if (is_playing_motion_)
		{
			current_frame_ = (current_frame_ - motion.StartFrame + 1) % (motion.EndFrame + 1 - motion.StartFrame) + motion.StartFrame;
		}

		auto& avatar = motion.Frames[current_frame_];
		animator_->UpdateBones(avatar);
	}
	
	root_node_->RecursiveUpdateMatrix(m_pGameObj->GetTransformComponent()->GetMatrix());

	if (animator_)
	{
		animator_->UpdateClusterWorld();
		root_node_->RecursiveUpdateSkin(animator_->Clusters);
	}
}

//--------------------------------------------------------------------------------
//  更新
//--------------------------------------------------------------------------------
void CModelAnalyzerBehaviorComponent::LateUpdate(void)
{
	// Menu
	ShowMainMenuBar();

	// Main Window
	ShowMainWindow();

	// Model Info Window
	ShowModelInfoWindow();

	// Edit Node Now
	ShowNodeNowWindow();

	// Animator
	ShowAnimatorWindow();

	// Material
	ShowMaterialWindow();

	// Camera
	ShowCameraWindow();
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
		if (root_node_)
		{
			auto nID = MessageBox(NULL, "モデルを切り替えますか？", "確認", MB_YESNO | MB_ICONWARNING | MB_DEFBUTTON2);
			if (nID == IDNO) { return; }

			//前のモデルの削除
			ReleaseModel();
		}

		//ImGui Flag
		is_display_model_window_ = false;
		is_display_animator_window_ = false;
		is_display_camera_window_ = false;
		current_frame_ = 0;
		motion_no_ = 0;

		//LoadModel
		file_name_ = strName;
		MyModel myModel;
		if (strType._Equal("txt"))
		{
			myModel = CKFUtilityFBX::LoadFromTxt(strFilePath);
		}
		else
		{
			myModel = CKFUtilityFBX::Load(strFilePath);
		}
		root_node_ = myModel.pNode;
		animator_ = myModel.pAnimator;
		materials_ = myModel.mapMaterial;
		for (auto& pair : materials_)
		{
			CMain::GetManager()->GetTextureManager()->UseTexture(pair.second.ColorTexture);
		}
		root_node_->RecursiveRecombineMeshes();
		root_node_->RecursivePush(node_names_, nodes_);
		if (animator_)
		{
			root_node_->RecursiveMatchClusterID(myModel.pAnimator->Clusters);
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
	model.pAnimator = animator_;
	model.pNode = root_node_;
	model.mapMaterial = materials_;
	if (CKFUtilityFBX::Save(model, file_name_, type))
	{
		is_saved_ = true;
		MessageBox(NULL, "セーブしました。", "SaveModel", MB_OK);
	}
}

//--------------------------------------------------------------------------------
//
//  Private
//
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
// ReleaseModel
//--------------------------------------------------------------------------------
void CModelAnalyzerBehaviorComponent::ReleaseModel(void)
{
	if (!is_saved_ && root_node_)
	{//Save確認
		auto nID = MessageBox(NULL, "今のモデルセーブしますか？", "確認", MB_YESNO | MB_ICONWARNING | MB_DEFBUTTON2);
		if (nID == IDYES) 
		{
			auto nID = MessageBox(NULL, "Jsonでセーブしますか？(Noの場合Binaryでセーブします)", "確認", MB_YESNO | MB_ICONWARNING | MB_DEFBUTTON2);
			SaveModel(nID == IDYES ? Json : Binary);
		}
	}
	node_names_.clear();
	nodes_.clear();
	SAFE_RELEASE(root_node_);
	SAFE_RELEASE(animator_);
	for (auto& pair : materials_)
	{
		CMain::GetManager()->GetTextureManager()->DisuseTexture(pair.second.ColorTexture);
	}
	materials_.clear();
	Init();
}

//--------------------------------------------------------------------------------
// mainWindow
//--------------------------------------------------------------------------------
void CModelAnalyzerBehaviorComponent::ShowMainWindow(void)
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
	if (root_node_)
	{
		if (ImGui::Button(is_display_model_window_ ? 
			"Close model window" :
			"Open model window")) is_display_model_window_ ^= 1;

		// Animator Window
		if (animator_)
		{
			if (ImGui::Button(is_display_animator_window_ ?
				"Close animator window" :
				"Open animator window")) is_display_animator_window_ ^= 1;
		}

		// Material Window
		if (ImGui::Button(is_display_material_window_ ?
			"Close material window" :
			"Open material window")) is_display_material_window_ ^= 1;
	}

	// Camera Window
	if (ImGui::Button(is_display_camera_window_ ?
		"Close camera window" :
		"Open camera window")) is_display_camera_window_ ^= 1;

	// DrawFlag
	if (ImGui::Button(is_render_skeletons_ ? 
		"Disdraw skeleton" : "Draw skeleton")) is_render_skeletons_ ^= 1;
	if (ImGui::Button(is_render_meshes_ ?
		"Disdraw mesh" : "Draw mesh")) is_render_meshes_ ^= 1;
	if (ImGui::Button(is_render_colliders_ ?
		"Disdraw collider" : "Draw collider")) is_render_colliders_ ^= 1;

	// End
	ImGui::End();
}

//--------------------------------------------------------------------------------
// ShowMainWindowMenuBar
//--------------------------------------------------------------------------------
void CModelAnalyzerBehaviorComponent::ShowMainMenuBar(void)
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			ShowMainMenuFile();
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
}

//--------------------------------------------------------------------------------
// ShowMainWindowFileMenu
//--------------------------------------------------------------------------------
void CModelAnalyzerBehaviorComponent::ShowMainMenuFile(void)
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
void CModelAnalyzerBehaviorComponent::ShowModelInfoWindow(void)
{
	if (!is_display_model_window_) return;

	// Begin
	if (!ImGui::Begin("ModelInfoWindow", &is_display_model_window_))
	{
		ImGui::End();
		return;
	}

	// Model Name
	char buffer[kBufferSize] = {};
	strcpy_s(buffer, file_name_.c_str());
	if (ImGui::InputText("Name", buffer, kBufferSize))
	{
		file_name_ = buffer;
	}

	// RotSpeed
	ImGui::InputFloat("RotationSpeed", &rotation_speed_);

	// Reverse Tex V
	if (ImGui::Checkbox("Reverse TextureV", &is_reserve_texcoordv_))
	{
		if (root_node_) { root_node_->RecursiveReverseTexV(); }
	}

	// Node Info
	if (ImGui::CollapsingHeader("Node Info"))
	{
		root_node_ = ShowNodeInfo(root_node_);
	}

	// End
	ImGui::End();
}

//--------------------------------------------------------------------------------
// ShowNodeInfo
//--------------------------------------------------------------------------------
CMyNode* CModelAnalyzerBehaviorComponent::ShowNodeInfo(CMyNode* pNode)
{
	if (!pNode) return nullptr;

	bool isDeleteNode = false;
	bool isChangeParent = false;
	if (ImGui::TreeNode(pNode->Name.c_str()))
	{
		if (ImGui::CollapsingHeader("Info"))
		{
			//Delete
			if (ImGui::Button("Delete")) isDeleteNode ^= 1;

			//Change Parent
			if (pNode->Parent)
			{
				if (ChangeParent(pNode) == true)
				{
					isChangeParent = true;
				}
			}

			//Type
			int size = (int)pNode->AttributeNames.size();
			for (int count = 0; count < size; ++count)
			{
				ImGui::Text("Type%d : %s", count, pNode->AttributeNames[count].c_str());
			}

			//Offset
			ImGui::InputFloat3("Transform", &pNode->Translation.m_fX);
			ImGui::DragFloat3("Rotation", &pNode->RotationOffset.m_fX, rotation_speed_, 0.0f, KF_PI * 2.0f);
			ImGui::InputFloat4("Rotation Quaternion", &pNode->Rotation.m_fX);
			ImGui::InputFloat3("Scaling", &pNode->Scale.m_fX);

			//Mesh
			if (!pNode->Meshes.empty() && ImGui::TreeNode("Mesh"))
			{
				for (int count = 0; count < (int)pNode->Meshes.size();)
				{
					auto& mesh = pNode->Meshes[count];
					auto& strMeshName = to_string(count);
					bool isDelete = false;
					if (ImGui::TreeNode(strMeshName.c_str()))
					{
						//delete
						if (ImGui::Button("Delete")) isDelete ^= 1;

						//delete skin
						if (ImGui::Button("Disattach bone"))
						{
							Mesh::DisAttachToBone(mesh);
						}

						//Info
						ImGui::Text("PolygonNumber : %d", mesh.PolygonNumber);
						ImGui::Text("VertexNumber : %d  IndexNumber : %d", mesh.VertexNumber, mesh.IndexNumber);

						//Material
						char buffer[kBufferSize] = {};
						strcpy_s(buffer, mesh.MaterialName.c_str());
						if (ImGui::InputText("MaterialName", buffer, kBufferSize))
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
						ImGui::Checkbox("Enable Light(for editor)", &mesh.EnableLight);

						// CullFace
						ImGui::Checkbox("Enable CullFace(for editor", &mesh.EnableCullFace);

						// CastShadow
						ImGui::Checkbox("Cast Shadow", &mesh.CastShadow);

						// ShaderType
						static const char* listbox_st[kShaderMax] =
						{
							"DefaultShader",
							"NoLightNoFog",
							"CullNone",
							"DefaultSkinShader",
							"JuggernautMeshShader",
							"JuggernautSkinShader"
						};
						ImGui::ListBox("Shader Type", (int*)&mesh.MyShaderType, listbox_st, kShaderMax, kShaderMax);
						ImGui::TreePop();
					}

					if (isDelete)
					{
						pNode->Meshes.erase(pNode->Meshes.begin() + count);
					}
					else
					{
						++count;
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
			if (!current_node_ && ImGui::Button("Edit Node"))
			{
				current_node_ = pNode;
				current_node_->ColliderMaterialID = 2;
			}
		}

		if (!pNode->Children.empty() && ImGui::CollapsingHeader("Child"))
		{
			//Child
			for (auto iterator = pNode->Children.begin(); iterator != pNode->Children.end();)
			{
				if (ShowNodeInfo(*iterator) == nullptr)
				{
					iterator = pNode->Children.erase(iterator);
				}
				else
				{
					++iterator;
				}
			}
		}
		ImGui::TreePop();
	}

	if (isDeleteNode)
	{
		pNode->Release();
		return nullptr;
	}
	if (isChangeParent)
	{
		return nullptr;
	}
	return pNode;
}

//--------------------------------------------------------------------------------
// ShowNodeInfo
//--------------------------------------------------------------------------------
void CModelAnalyzerBehaviorComponent::ShowNodeNowWindow(void)
{
	if (!current_node_) { return; }
	bool bNodeNow = true;

	// Begin
	if (!ImGui::Begin("Edit Node Now Window", &bNodeNow))
	{
		ImGui::End();
		return;
	}

	// Model Name
	char buffer[kBufferSize] = {};
	strcpy_s(buffer, file_name_.c_str());
	if (ImGui::InputText("Name", buffer, kBufferSize))
	{
		current_node_->Name = buffer;
	}

	// Collider
	if (ImGui::CollapsingHeader("Collider"))
	{
		//Edit Collider
		static const char* listbox_items[] =
		{ "Sphere"
			, "AABB"
			, "OBB" };
		int count = 0;
		for (auto itr = current_node_->Colliders.begin(); itr != current_node_->Colliders.end();)
		{
			bool bDelete = false;
			char aBuf[128];
			wsprintf(aBuf, "Collider%d", count);
			if (ImGui::TreeNode(aBuf))
			{
				//Type
				ImGui::ListBox("Collider Type\n(single select)", (int*)&itr->Type, listbox_items, 3, 3);
				
				//Offset
				ImGui::InputFloat3("Translation", &itr->Position.m_fX);
				ImGui::DragFloat3("Rotation", &itr->Rotation.m_fX, rotation_speed_, 0.0f, KF_PI * 2.0f);
				ImGui::InputFloat3("Scale", &itr->Scale.m_fX);

				if (itr->Type == CS::COL_SPHERE)
				{//same scale xyz by x
					itr->Scale.m_fY =
						itr->Scale.m_fZ =
						itr->Scale.m_fX;
				}

				//IsCollider
				ImGui::Checkbox("IsTrigger", &itr->IsTrigger);

				bDelete = ImGui::Button("Delete Collider");
				ImGui::TreePop();
			}

			if(bDelete)
			{
				itr = current_node_->Colliders.erase(itr);
			}
			else
			{
				++itr;
				++count;
			}
		}

		//Add Collider
		if (ImGui::Button("AddCollider"))
		{
			ColliderInfo col;
			col.Type = CS::COL_SPHERE;
			col.Position = CKFMath::sc_vZero;
			col.Rotation = CKFMath::sc_vZero;
			col.Scale = CKFMath::sc_vOne;
			col.IsTrigger = false;
			current_node_->Colliders.push_back(col);
		}
	}

	// Fix Vtx
	if (ImGui::CollapsingHeader("Fix Verteces"))
	{
		//Offset
		ImGui::InputFloat3("Fixed Trans", &current_node_correct_translation_.m_fX);
		ImGui::InputFloat3("Fixed Rot", &current_node_correct_rotation_.m_fX);
		ImGui::InputFloat3("Fixed Scale", &current_node_correct_scale_.m_fX);

		// Recalculate By Mtx
		if (ImGui::Button("FixVerteces"))
		{
			CKFMtx44 mtxThis;

			//拡縮
			mtxThis.m_af[0][0] = current_node_correct_scale_.m_fX;
			mtxThis.m_af[1][1] = current_node_correct_scale_.m_fY;
			mtxThis.m_af[2][2] = current_node_correct_scale_.m_fZ;

			//回転
			CKFMtx44 mtxRot;
			CKFMath::MtxRotationYawPitchRoll(mtxRot, current_node_correct_rotation_);
			mtxThis *= mtxRot;

			//平行移動
			CKFMtx44 mtxPos;
			CKFMath::MtxTranslation(mtxPos, current_node_correct_translation_);
			mtxThis *= mtxPos;

			//Recalculate
			current_node_->RecalculateMeshesBy(mtxThis);

			//Reset
			current_node_correct_translation_ = CKFMath::sc_vZero;
			current_node_correct_rotation_ = CKFMath::sc_vZero;
			current_node_correct_scale_ = CKFMath::sc_vOne;
		}
	}

	// End
	ImGui::End();

	if (!bNodeNow) 
	{
		current_node_->ColliderMaterialID = 1;
		current_node_ = nullptr;
		current_node_correct_translation_ = CKFMath::sc_vZero;
		current_node_correct_rotation_ = CKFMath::sc_vZero;
		current_node_correct_scale_ = CKFMath::sc_vOne;
	}
}

//--------------------------------------------------------------------------------
// ShowAnimatorWindow
//--------------------------------------------------------------------------------
void CModelAnalyzerBehaviorComponent::ShowAnimatorWindow(void)
{
	if (!is_display_animator_window_) return;

	// Begin
	if (!ImGui::Begin("Animator Window", &is_display_animator_window_))
	{
		ImGui::End();
		return;
	}

	// IK
	if (ImGui::CollapsingHeader("IKController"))
	{
		static const char* IKName[eIKMax] =
		{
			"Hips",
			"Spine",
			"ShoulderLeft",
			"UpperArmLeft",
			"LowerArmLeft",
			"HandLeft",
			"ShoulderRight",
			"UpperArmRight",
			"LowerArmRight",
			"HandRight",
			"UpperLegLeft",
			"LowerLegLeft",
			"FootLeft",
			"ToesLeft",
			"UpperLegRight",
			"LowerLegRight",
			"FootRight",
			"ToesRight",
		};

		for (int count = 0; count < eIKMax; ++count)
		{
			ImGui::Combo(IKName[count], (int*)&animator_->IKControllers[count].index, node_names_);
		}
	}

	// Motion
	if (ImGui::CollapsingHeader("Motion"))
	{
		if (!animator_->Motions.empty())
		{
			// Play
			if (ImGui::Button(is_playing_motion_ ? "Pause" : "Play")) is_playing_motion_ ^= 1;

			// Frame
			auto& current = animator_->Motions[motion_no_];
			ImGui::SliderInt("Frame", &current_frame_, current.StartFrame, current.EndFrame);
			
			// Type
			vector<string> labels;
			labels.reserve(animator_->Motions.size());
			for (auto& motion : animator_->Motions)
			{
				labels.push_back(motion.Name);
			}
			if (ImGui::Combo("Select Motion", (int*)&motion_no_, labels))
			{
				current_frame_ = 0;
			}
			labels.clear();

			// Animation Current
			ShowCurrentAnimationWindow();
		}

		// AddAnimation
		if (ImGui::Button("Add animation"))
		{
			AddAnimation();
		}
	}
	
	// End
	ImGui::End();
}

//--------------------------------------------------------------------------------
// マテリアルのウインドウ表示
//--------------------------------------------------------------------------------
void CModelAnalyzerBehaviorComponent::ShowMaterialWindow(void)
{
	if (!is_display_material_window_) return;

	// Begin
	if (!ImGui::Begin("Material Window", &is_display_material_window_))
	{
		ImGui::End();
		return;
	}

	// マテリアルの編集
	if (ImGui::CollapsingHeader("Materials"))
	{
		for (auto iterator = materials_.begin(); iterator != materials_.end();)
		{
			bool isDelete = false;
			if (ImGui::TreeNode(iterator->first.c_str()))
			{
				ImGui::ColorEdit3("Diffuse", (float*)&iterator->second.Diffuse);
				ImGui::ColorEdit3("Ambient", (float*)&iterator->second.Ambient);
				ImGui::ColorEdit3("Specular", (float*)&iterator->second.Specular);
				ImGui::ColorEdit3("Emissive", (float*)&iterator->second.Emissive);
				ImGui::InputFloat("Power", &iterator->second.Power);
				ImGui::Text("ColorTexture : %s", iterator->second.ColorTexture.c_str());
				if (ImGui::Button("Change ColorTexture"))
				{
					ChangeTexture(iterator->second.ColorTexture);
				}
				ImGui::Text("DiffuseTexture : %s", iterator->second.DiffuseTexture.c_str());
				if (ImGui::Button("Change DiffuseTexture"))
				{
					ChangeTexture(iterator->second.DiffuseTexture);
				}
				ImGui::Text("DiffuseTextureMask : %s", iterator->second.DiffuseTextureMask.c_str());
				if (ImGui::Button("Change DiffuseTextureMask"))
				{
					ChangeTexture(iterator->second.DiffuseTextureMask);
				}
				ImGui::Text("SpecularTexture : %s", iterator->second.SpecularTexture.c_str());
				if (ImGui::Button("Change SpecularTexture"))
				{
					ChangeTexture(iterator->second.SpecularTexture);
				}
				ImGui::Text("SpecularTextureMask : %s", iterator->second.SpecularTextureMask.c_str());
				if (ImGui::Button("Change SpecularTextureMask"))
				{
					ChangeTexture(iterator->second.SpecularTextureMask);
				}
				ImGui::Text("NormalTexture : %s", iterator->second.NormalTexture.c_str());
				if (ImGui::Button("Change NormalTexture"))
				{
					ChangeTexture(iterator->second.NormalTexture);
				}
				if(ImGui::Button("Delete")) isDelete = true;
				ImGui::TreePop();
			}

			if (isDelete == true)
			{
				iterator = materials_.erase(iterator);
			}
			else
			{
				++iterator;
			}
		}
	}
	
	// マテリアルの追加
	if (ImGui::CollapsingHeader("Add Material"))
	{
		static string name;
		static Material material;

		char buffer[kBufferSize] = {};
		strcpy_s(buffer, name.c_str());
		if (ImGui::InputText("EditName", buffer, kBufferSize))
		{
			name = buffer;
		}

		ImGui::ColorEdit3("Diffuse", (float*)&material.Diffuse);
		ImGui::ColorEdit3("Ambient", (float*)&material.Ambient);
		ImGui::ColorEdit3("Specular", (float*)&material.Specular);
		ImGui::ColorEdit3("Emissive", (float*)&material.Emissive);
		ImGui::DragFloat("Power", &material.Power);
		ImGui::Text("ColorTexture : %s", material.ColorTexture.c_str());
		if (ImGui::Button("Change ColorTexture"))
		{
			ChangeTexture(material.ColorTexture);
		}
		ImGui::Text("DiffuseTexture : %s", material.DiffuseTexture.c_str());
		if (ImGui::Button("Change DiffuseTexture"))
		{
			ChangeTexture(material.DiffuseTexture);
		}
		ImGui::Text("DiffuseTextureMask : %s", material.DiffuseTextureMask.c_str());
		if (ImGui::Button("Change DiffuseTextureMask"))
		{
			ChangeTexture(material.DiffuseTextureMask);
		}
		ImGui::Text("SpecularTexture : %s", material.SpecularTexture.c_str());
		if (ImGui::Button("Change SpecularTexture"))
		{
			ChangeTexture(material.SpecularTexture);
		}
		ImGui::Text("SpecularTextureMask : %s", material.SpecularTextureMask.c_str());
		if (ImGui::Button("Change SpecularTextureMask"))
		{
			ChangeTexture(material.SpecularTextureMask);
		}
		ImGui::Text("NormalTexture : %s", material.NormalTexture.c_str());
		if (ImGui::Button("Change NormalTexture"))
		{
			ChangeTexture(material.NormalTexture);
		}
		if (ImGui::Button("Add to Materials"))
		{
			materials_.emplace(name, material);
			if (material.DiffuseTexture.empty())
			{
				CMain::GetManager()->GetTextureManager()->UseTexture(material.ColorTexture);
			}
			name.clear();
			material.ColorTexture.clear();
			material.DiffuseTexture.clear();
			material.DiffuseTextureMask.clear();
			material.NormalTexture.clear();
			material.SpecularTexture.clear();
			material.SpecularTextureMask.clear();
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
// ShowCameraWindow
//--------------------------------------------------------------------------------
void CModelAnalyzerBehaviorComponent::ShowCameraWindow(void)
{
	if (!is_display_camera_window_) { return; }

	// Begin
	if (!ImGui::Begin("Camera Window", &is_display_camera_window_))
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
// ShowCurrentAnimationWindow
//--------------------------------------------------------------------------------
void CModelAnalyzerBehaviorComponent::ShowCurrentAnimationWindow(void)
{
	// Motion
	if (animator_->Motions.empty()) return;
	bool isDelete = false;
	if (ImGui::CollapsingHeader("CurrentAnimation"))
	{
		auto& current = animator_->Motions[motion_no_];
		
		// 名前
		char buffer[kBufferSize] = {};
		strcpy_s(buffer, current.Name.c_str());
		if (ImGui::InputText("Name", buffer, kBufferSize))
		{
			current.Name = buffer;
		}

		// フレーム編集
		if (ImGui::TreeNode("Edit Frame"))
		{
			// Start Frame
			if (ImGui::SliderInt("StartFrame", &current.StartFrame, 0, current.EndFrame))
			{
				if (current_frame_ < current.StartFrame)
				{
					current_frame_ = current.StartFrame;
				}
			}

			// End Frame
			if (ImGui::SliderInt("EndFrame", &current.EndFrame, current.StartFrame, (int)current.Frames.size() - 1))
			{
				if (current_frame_ > current.EndFrame)
				{
					current_frame_ = current.EndFrame;
				}
			}

			// Delete Frame that out of range
			if (ImGui::Button("Delete out of range frames"))
			{
				animator_->DeleteOutOfRangeFrames(motion_no_);
				current_frame_ = current.StartFrame;
			}

			ImGui::TreePop();
		}

		// モーション切り替え設定
		if (ImGui::TreeNode("StateTransition"))
		{
			ImGui::Checkbox("IsLoop", &current.IsLoop);

			ImGui::Checkbox("Change when is over\nNotice : Unavailable when the IsLoop flag is true", &current.ChangeWhenOver);

			if (!current.IsLoop && current.ChangeWhenOver)
			{
				ImGui::SliderInt("The frame we think its' over", &current.ChangeWhenOverExitFrame, current.StartFrame, current.EndFrame);
				ImGui::InputInt("The blend frame number", &current.ChangeWhenOverBlendFrame);
				char nameBuffer[kBufferSize] = {};
				strcpy_s(nameBuffer, current.ChangeWhenOverNextMotion.c_str());
				if (ImGui::InputText("Next animation name", nameBuffer, kBufferSize))
				{
					current.ChangeWhenOverNextMotion = nameBuffer;
				}
			}

			int countTransition = 0;
			for (auto iterator = current.Transitions.begin(); iterator != current.Transitions.end();)
			{
				bool isDeleteTransition = false;
				string& treeName = "Transition" + std::to_string(countTransition);
				if (ImGui::TreeNode(treeName.c_str()))
				{
					// 次のモーション名
					char nameBuffer[kBufferSize] = {};
					strcpy_s(nameBuffer, iterator->NextMotion.c_str());
					if (ImGui::InputText("Next animation name", nameBuffer, kBufferSize))
					{
						iterator->NextMotion = nameBuffer;
					}

					// Blend frame
					ImGui::InputInt("The blend frame number", &iterator->BlendFrame);

					// 条件
					int countCondition = 0;
					for (auto itrCondition = iterator->Conditions.begin(); itrCondition != iterator->Conditions.end();)
					{
						bool isDeleteCondition = false;
						string& conditionTreeName = "Condition" + std::to_string(countCondition);
						if (ImGui::TreeNode(conditionTreeName.c_str()))
						{
							// 条件変数の型
							static const char* typeLabels[] =
							{
								"bool",
								"float",
							};
							ImGui::Combo("ParameterType", (int*)&itrCondition->ParameterType, typeLabels, 2);

							// 条件変数名
							char parameterNameBuffer[kBufferSize] = {};
							strcpy_s(parameterNameBuffer, itrCondition->ParameterName.c_str());
							if (ImGui::InputText("ParameterName", parameterNameBuffer, kBufferSize))
							{
								itrCondition->ParameterName = parameterNameBuffer;
							}

							// オペレーター
							if (itrCondition->ParameterType == eParameterType::eFloat)
							{
								static const char* operatorLabels[] =
								{
									"equal",
									"notEqual",
									"greater",
									"less"
								};
								ImGui::Combo("Operator", (int*)&itrCondition->FloatOperator, operatorLabels, 4);
							}

							// 値
							switch (itrCondition->ParameterType)
							{
							case eParameterType::eBool:
							{
								static const char* valueLabels[] =
								{
									"isFalse",
									"isTrue",
								};
								ImGui::Combo("Value", (int*)&itrCondition->BoolValue, valueLabels, 2);
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
							++countCondition;
							++itrCondition;
						}
					}

					// 条件の追加
					if (ImGui::Button("Add Condition"))
					{
						iterator->Conditions.push_back(Condition());
					}

					// 切り替えの削除
					if (ImGui::Button("Delete Transition"))
					{
						isDeleteTransition = true;
					}
					ImGui::TreePop();
				}

				// Count iterator
				if (isDeleteTransition)
				{
					iterator = current.Transitions.erase(iterator);
				}
				else
				{
					++countTransition;
					++iterator;
				}
			}

			// 切り替えの追加
			if (ImGui::Button("Add Transition"))
			{
				current.Transitions.push_back(StateTransition());
			}

			// Save Transition
			if (ImGui::Button("Save Transitions"))
			{
				animator_->SaveMotionTransitions(motion_no_);
			}

			ImGui::TreePop();
		}

		// Delete Animation
		if (ImGui::Button("Delete animation"))
		{
			isDelete ^= 1;
		}
	}
	if (isDelete)
	{
		animator_->Motions.erase(animator_->Motions.begin() + motion_no_);
		motion_no_ = 0;
		current_frame_ = 0;
	}
}

//--------------------------------------------------------------------------------
// ChangeTexture
//--------------------------------------------------------------------------------
void CModelAnalyzerBehaviorComponent::ChangeTexture(string& meshTexture)
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
// AddAnimation
//--------------------------------------------------------------------------------
void CModelAnalyzerBehaviorComponent::AddAnimation(void)
{
	string strFileName;
	if (CMain::OpenTextureFile(strFileName))
	{
		string strName, strType;
		CKFUtility::AnalyzeFilePath(strFileName, strName, strType);
		if (strType._Equal("fbx") || strType._Equal("FBX"))
		{
			CKFUtilityFBX::LoadAnimation(strFileName, animator_);
		}
	}
}

//--------------------------------------------------------------------------------
// ShowChangeParent
//--------------------------------------------------------------------------------
bool CModelAnalyzerBehaviorComponent::ChangeParent(CMyNode* pNode)
{
	int index = 0;
	if (ImGui::Combo("Change Parent", (int*)&index, node_names_))
	{
		if (nodes_[index] != pNode
			&& nodes_[index] != pNode->Parent)
		{
			if (pNode->RecursiveCheckIsChild(nodes_[index]) == false)
			{
				pNode->ChangeParent(nodes_[index]);
				return true;
			}
		}
	}
	return false;
}