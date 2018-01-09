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
	, is_render_bounding_sphere(true)
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
	, current_language_(eEnglish)
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
	string name, type;
	CKFUtility::AnalyzeFilePath(strFilePath, name, type);
	if (type._Equal("FBX")
		|| type._Equal("fbx")
		|| type._Equal("obj")
		|| type._Equal("OBJ")
		|| type._Equal("txt"))
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
		file_name_ = name;
		MyModel myModel;
		if (type._Equal("txt"))
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
	current_node_ = nullptr;
	materials_.clear();
	Init();
}

//--------------------------------------------------------------------------------
// mainWindow
//--------------------------------------------------------------------------------
void CModelAnalyzerBehaviorComponent::ShowMainWindow(void)
{
	// Begin
	if (!ImGui::Begin("Main window"))
	{
		ImGui::End();
		return;
	}

	// Language
	ImGui::ListBox(kSelectLanguage[current_language_],
		(int*)&current_language_, kLanguage, eLanguageMax, eLanguageMax);

	// FPS
	ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

	// BG Color
	auto renderer = CMain::GetManager()->GetRenderer();
	auto background_color = renderer->GetBGColor();
	ImGui::ColorEdit3(kBackgroundColor[current_language_], (float*)&background_color);
	renderer->SetBGColor(background_color);

	// Model Window
	if (root_node_)
	{
		if (ImGui::Button(is_display_model_window_ ?
			kCloseModelWindow[current_language_] : kOpenModelWindow[current_language_]))
		{
			is_display_model_window_ ^= 1;
		}

		// Animator Window
		if (animator_)
		{
			if (ImGui::Button(is_display_animator_window_ ?
				kCloseAnimatorWindow[current_language_] : kOpenAnimatorWindow[current_language_]))
			{
				is_display_animator_window_ ^= 1;
			}
		}

		// Material Window
		if (ImGui::Button(is_display_material_window_ ?
			kCloseMaterialWindow[current_language_] : kOpenMaterialWindow[current_language_]))
		{
			is_display_material_window_ ^= 1;
		}
	}

	// Camera Window
	if (ImGui::Button(is_display_camera_window_ ?
		kCloseCameraWindow[current_language_] : kOpenCameraWindow[current_language_]))
	{
		is_display_camera_window_ ^= 1;
	}

	// DrawFlag
	if (ImGui::Button(is_render_skeletons_ ?
		kUndisplaySkeleton[current_language_] : kDisplaySkeleton[current_language_]))
	{
		is_render_skeletons_ ^= 1;
	}

	if (ImGui::Button(is_render_meshes_ ?
		kUndisplayMesh[current_language_] : kDisplayMesh[current_language_]))
	{
		is_render_meshes_ ^= 1;
	}

	if (ImGui::Button(is_render_colliders_ ?
		kUndisplayCollider[current_language_] : kDisplayCollider[current_language_]))
	{
		is_render_colliders_ ^= 1;
	}

	if (ImGui::Button(is_render_bounding_sphere ?
		kUndisplayBoundingSphere[current_language_] : kDisplayBoundingSphere[current_language_]))
	{
		is_render_bounding_sphere ^= 1;
	}

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
		if (ImGui::BeginMenu(kFile[current_language_]))
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
	if (ImGui::MenuItem(kOpenFbxFile[current_language_]))
	{
		string file_name;
		if (CMain::OpenModelFile(file_name))
		{
			ChangeModel(file_name);
		}
	}
	if (ImGui::MenuItem(kSaveAsJson[current_language_]))
	{
		SaveModel(Json);
	}
	if (ImGui::MenuItem(kSaveAsBinary[current_language_]))
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
	if (!ImGui::Begin("Model info window", &is_display_model_window_))
	{
		ImGui::End();
		return;
	}

	// Model Name
	char buffer[kBufferSize] = {};
	strcpy_s(buffer, file_name_.c_str());
	if (ImGui::InputText(kName[current_language_], buffer, kBufferSize))
	{
		file_name_ = buffer;
	}

	// RotSpeed
	ImGui::InputFloat(kRotationSpeed[current_language_], &rotation_speed_);

	// Reverse Tex V
	if (ImGui::Checkbox(kReverseTexcoordV[current_language_], &is_reserve_texcoordv_))
	{
		if (root_node_) { root_node_->RecursiveReverseTexV(); }
	}

	// Node Info
	if (ImGui::CollapsingHeader(kNodeInfo[current_language_]))
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

	bool is_delete_node = false;
	bool is_change_parent = false;
	if (ImGui::TreeNode(pNode->Name.c_str()))
	{
		if (ImGui::CollapsingHeader(kNodeInfo[current_language_]))
		{
			//Edit
			if (!current_node_ && ImGui::Button(kEditNode[current_language_]))
			{
				current_node_ = pNode;
				current_node_->ColliderMaterialID = 2;
			}

			//Delete
			if (ImGui::Button(kDelete[current_language_])) is_delete_node ^= 1;

			//Change Parent
			if (pNode->Parent)
			{
				if (ChangeParent(pNode) == true)
				{
					is_change_parent = true;
				}
			}

			//Type
			int size = (int)pNode->AttributeNames.size();
			for (int count = 0; count < size; ++count)
			{
				ImGui::Text(kType[current_language_], count, pNode->AttributeNames[count].c_str());
			}

			//Offset
			ImGui::InputFloat3(kTranslation[current_language_], &pNode->Translation.m_fX);
			ImGui::DragFloat3(kRotation[current_language_], &pNode->RotationOffset.m_fX, rotation_speed_, 0.0f, KF_PI * 2.0f);
			ImGui::InputFloat4(kQuaternion[current_language_], &pNode->Rotation.m_fX);
			ImGui::InputFloat3(kScale[current_language_], &pNode->Scale.m_fX);

			//Mesh
			if (!pNode->Meshes.empty() && ImGui::TreeNode(kMesh[current_language_]))
			{
				for (int count = 0; count < (int)pNode->Meshes.size();)
				{
					auto& mesh = pNode->Meshes[count];
					auto& mesh_name = to_string(count);
					bool is_delete = false;
					if (ImGui::TreeNode(mesh_name.c_str()))
					{
						// BoundingSphere
						ImGui::Text(kBoundingSphereInfo[current_language_],
							mesh.BoundingSpherePosition.m_fX,
							mesh.BoundingSpherePosition.m_fY,
							mesh.BoundingSpherePosition.m_fZ,
							mesh.BoundingSphereRadius);

						//delete
						if (ImGui::Button(kDelete[current_language_])) is_delete ^= 1;

						//delete skin
						if (ImGui::Button(kDisattachBone[current_language_]))
						{
							Mesh::DisAttachToBone(mesh);
						}

						//Info
						ImGui::Text(kPolygonNumber[current_language_], mesh.PolygonNumber);
						ImGui::Text(kVertexNumber[current_language_], mesh.VertexNumber);
						ImGui::Text(kIndexNumber[current_language_], mesh.IndexNumber);

						//Material
						char buffer[kBufferSize] = {};
						strcpy_s(buffer, mesh.MaterialName.c_str());
						if (ImGui::InputText(kMaterialName[current_language_], buffer, kBufferSize))
						{
							mesh.MaterialName = buffer;
						}

						// Render Priority
						static const char* listbox_rp[] =
						{ "3D"
							, "3D_ALPHATEST"
							, "3D_ZSORT" };
						ImGui::ListBox(kRenderPriority[current_language_], (int*)&mesh.MyRenderPriority, listbox_rp, 3, 3);

						// Light
						ImGui::Checkbox(kEnableLight[current_language_], &mesh.EnableLight);

						// CullFace
						ImGui::Checkbox(kEnableCullFace[current_language_], &mesh.EnableCullFace);

						// CastShadow
						ImGui::Checkbox(kCastShadow[current_language_], &mesh.CastShadow);

						// ShaderType
						static const char* listbox_st[kShaderMax] =
						{
							"DefaultShader",
							"NoLightNoFog",
							"CullNone",
							"DefaultSkinShader",
							"JuggernautMeshShader",
							"JuggernautSkinShader",
                            "ZombieSkinShader"
						};
						ImGui::ListBox(kShaderType[current_language_], (int*)&mesh.MyShaderType, listbox_st, kShaderMax, kShaderMax);
						ImGui::TreePop();
					}

					if (is_delete)
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
			ImGui::Text(kColliderNumber[current_language_], (int)pNode->Colliders.size());
		}

		if (!pNode->Children.empty() && ImGui::CollapsingHeader(kChildren[current_language_]))
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

	if (is_delete_node)
	{
		pNode->Release();
		return nullptr;
	}
	if (is_change_parent)
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
	bool is_current_node_window = true;

	// Begin
	if (!ImGui::Begin("Current node window", &is_current_node_window))
	{
		ImGui::End();
		return;
	}

	// Node Name
	char buffer[kBufferSize] = {};
	strcpy_s(buffer, file_name_.c_str());
	if (ImGui::InputText(kName[current_language_], buffer, kBufferSize))
	{
		current_node_->Name = buffer;
	}

	// Collider
	if (ImGui::CollapsingHeader(kCollider[current_language_]))
	{
		//Edit Collider
		static const char* listbox_items[] =
		{ "Sphere"
			, "Aabb"
			, "Obb" };
		int count = 0;
		for (auto itr = current_node_->Colliders.begin(); itr != current_node_->Colliders.end();)
		{
			bool is_delete = false;
			char buffer[128];
			wsprintf(buffer, "%d", count);
			if (ImGui::TreeNode(buffer))
			{
				//Type
				ImGui::ListBox(kType[current_language_], (int*)&itr->Type, listbox_items, 3, 3);
				
				//Offset
				ImGui::InputFloat3(kTranslation[current_language_], &itr->Position.m_fX);
				ImGui::DragFloat3(kRotation[current_language_], &itr->Rotation.m_fX, rotation_speed_, 0.0f, KF_PI * 2.0f);
				ImGui::InputFloat3(kScale[current_language_], &itr->Scale.m_fX);

				if (itr->Type == CS::COL_SPHERE)
				{//same scale xyz by x
					itr->Scale.m_fY =
						itr->Scale.m_fZ =
						itr->Scale.m_fX;
				}

				//IsCollider
				ImGui::Checkbox(kTrigger[current_language_], &itr->IsTrigger);

				is_delete = ImGui::Button(kDelete[current_language_]);
				ImGui::TreePop();
			}

			if(is_delete)
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
		if (ImGui::Button(kAdd[current_language_]))
		{
			ColliderInfo collider;
			collider.Type = CS::COL_SPHERE;
			collider.Position = CKFMath::sc_vZero;
			collider.Rotation = CKFMath::sc_vZero;
			collider.Scale = CKFMath::sc_vOne;
			collider.IsTrigger = false;
			current_node_->Colliders.push_back(collider);
		}
	}

	// Fix Vtx
	if (ImGui::CollapsingHeader(kTransformVerteces[current_language_]))
	{
		//Offset
		ImGui::InputFloat3(kTranslation[current_language_], &current_node_correct_translation_.m_fX);
		ImGui::InputFloat3(kRotation[current_language_], &current_node_correct_rotation_.m_fX);
		ImGui::InputFloat3(kScale[current_language_], &current_node_correct_scale_.m_fX);

		// Recalculate By Mtx
		if (ImGui::Button(kRecalculate[current_language_]))
		{
			CKFMtx44 world;

			//拡縮
			world.m_af[0][0] = current_node_correct_scale_.m_fX;
			world.m_af[1][1] = current_node_correct_scale_.m_fY;
			world.m_af[2][2] = current_node_correct_scale_.m_fZ;

			//回転
			CKFMtx44 rotation;
			CKFMath::MtxRotationYawPitchRoll(rotation, current_node_correct_rotation_);
			world *= rotation;

			//平行移動
			CKFMtx44 translation;
			CKFMath::MtxTranslation(translation, current_node_correct_translation_);
			world *= translation;

			//Recalculate
			current_node_->RecalculateMeshesBy(world);

			//Reset
			current_node_correct_translation_ = CKFMath::sc_vZero;
			current_node_correct_rotation_ = CKFMath::sc_vZero;
			current_node_correct_scale_ = CKFMath::sc_vOne;
		}
	}

	// End
	ImGui::End();

	if (!is_current_node_window)
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
	if (!ImGui::Begin("Animator window", &is_display_animator_window_))
	{
		ImGui::End();
		return;
	}

	// IK
	if (ImGui::CollapsingHeader(kIKController[current_language_]))
	{
		static const char* ik_name[eIKMax] =
		{
			"Head",
			"Neck",
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
			ImGui::Combo(ik_name[count], (int*)&animator_->IKControllers[count].index, animator_->ClusterNames);
		}
	}

	// Motion
	if (ImGui::CollapsingHeader(kMotion[current_language_]))
	{
		if (!animator_->Motions.empty())
		{
			// Play
			if (ImGui::Button(is_playing_motion_ ? kPause[current_language_] : kPlay[current_language_])) is_playing_motion_ ^= 1;

			// Frame
			auto& current = animator_->Motions[motion_no_];
			ImGui::SliderInt(kFrame[current_language_], &current_frame_, current.StartFrame, current.EndFrame);
			
			// Type
			vector<string> labels;
			labels.reserve(animator_->Motions.size());
			for (auto& motion : animator_->Motions)
			{
				labels.push_back(motion.Name);
			}
			if (ImGui::Combo(kSelectMotion[current_language_], (int*)&motion_no_, labels))
			{
				current_frame_ = 0;
			}
			labels.clear();

			// Animation Current
			ShowCurrentAnimationWindow();
		}

		// AddAnimation
		if (ImGui::Button(kAddAnimation[current_language_]))
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
	if (ImGui::CollapsingHeader(kMaterials[current_language_]))
	{
		for (auto iterator = materials_.begin(); iterator != materials_.end();)
		{
			bool is_delete = false;
			if (ImGui::TreeNode(iterator->first.c_str()))
			{
				ImGui::ColorEdit3(kDiffuse[current_language_], (float*)&iterator->second.Diffuse);
				ImGui::ColorEdit3(kAmbient[current_language_], (float*)&iterator->second.Ambient);
				ImGui::ColorEdit3(kSpecular[current_language_], (float*)&iterator->second.Specular);
				ImGui::ColorEdit3(kEmissive[current_language_], (float*)&iterator->second.Emissive);
				ImGui::InputFloat(kPower[current_language_], &iterator->second.Power);
				ImGui::Text(kColorTextureName[current_language_], iterator->second.ColorTexture.c_str());
				if (ImGui::Button(kChangeColorTexture[current_language_]))
				{
					ChangeTexture(iterator->second.ColorTexture);
				}
				ImGui::Text(kDiffuseTextureName[current_language_], iterator->second.DiffuseTexture.c_str());
				if (ImGui::Button(kChangeDiffuseTexture[current_language_]))
				{
					ChangeTexture(iterator->second.DiffuseTexture);
				}
				ImGui::Text(kDiffuseMaskName[current_language_], iterator->second.DiffuseTextureMask.c_str());
				if (ImGui::Button(kChangeDiffuseMask[current_language_]))
				{
					ChangeTexture(iterator->second.DiffuseTextureMask);
				}
				ImGui::Text(kSpecularTextureName[current_language_], iterator->second.SpecularTexture.c_str());
				if (ImGui::Button(kChangeSpecularTexture[current_language_]))
				{
					ChangeTexture(iterator->second.SpecularTexture);
				}
				ImGui::Text(kSpecularMaskName[current_language_], iterator->second.SpecularTextureMask.c_str());
				if (ImGui::Button(kChangeSpecularMask[current_language_]))
				{
					ChangeTexture(iterator->second.SpecularTextureMask);
				}
				ImGui::Text(kNormalTextureName[current_language_], iterator->second.NormalTexture.c_str());
				if (ImGui::Button(kChangeNormalTexture[current_language_]))
				{
					ChangeTexture(iterator->second.NormalTexture);
				}
				ImGui::Text(kDetailTextureName[current_language_], iterator->second.DetailTexture.c_str());
				if (ImGui::Button(kChangeDetailTexture[current_language_]))
				{
					ChangeTexture(iterator->second.DetailTexture);
				}
				ImGui::Text(kDetailMaskName[current_language_], iterator->second.DetailMask.c_str());
				if (ImGui::Button(kChangeDetailMask[current_language_]))
				{
					ChangeTexture(iterator->second.DetailMask);
				}
				ImGui::Text(kTintByBaseMaskName[current_language_], iterator->second.TintByBaseMask.c_str());
				if (ImGui::Button(kChangeTintByBaseMask[current_language_]))
				{
					ChangeTexture(iterator->second.TintByBaseMask);
				}
				ImGui::Text(kRimMaskName[current_language_], iterator->second.RimMask.c_str());
				if (ImGui::Button(kChangeRimMask[current_language_]))
				{
					ChangeTexture(iterator->second.RimMask);
				}
				ImGui::Text(kTranslucency[current_language_], iterator->second.Translucency.c_str());
				if (ImGui::Button(kChangeTranslucency[current_language_]))
				{
					ChangeTexture(iterator->second.Translucency);
				}
				ImGui::Text(kMetalnessMask[current_language_], iterator->second.MetalnessMask.c_str());
				if (ImGui::Button(kChangeMetalnessMask[current_language_]))
				{
					ChangeTexture(iterator->second.MetalnessMask);
				}
				ImGui::Text(kSelfIllumMask[current_language_], iterator->second.SelfIllumMask.c_str());
				if (ImGui::Button(kChangeSelfIllumMask[current_language_]))
				{
					ChangeTexture(iterator->second.SelfIllumMask);
				}
				ImGui::Text(kFresnelWarpColor[current_language_], iterator->second.FresnelWarpColor.c_str());
				if (ImGui::Button(kChangeFresnelWarpColor[current_language_]))
				{
					ChangeTexture(iterator->second.FresnelWarpColor);
				}
				ImGui::Text(kFresnelWarpRim[current_language_], iterator->second.FresnelWarpRim.c_str());
				if (ImGui::Button(kChangeFresnelWarpRim[current_language_]))
				{
					ChangeTexture(iterator->second.FresnelWarpRim);
				}
				ImGui::Text(kFresnelWarpSpecular[current_language_], iterator->second.FresnelWarpSpecular.c_str());
				if (ImGui::Button(kChangeFresnelWarpSpecular[current_language_]))
				{
					ChangeTexture(iterator->second.FresnelWarpSpecular);
				}
				if(ImGui::Button(kDelete[current_language_])) is_delete = true;
				ImGui::TreePop();
			}

			if (is_delete == true)
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
	if (ImGui::CollapsingHeader(kNewMaterial[current_language_]))
	{
		static string name;
		static Material material;

		char buffer[kBufferSize] = {};
		strcpy_s(buffer, name.c_str());
		if (ImGui::InputText(kName[current_language_], buffer, kBufferSize))
		{
			name = buffer;
		}

		ImGui::ColorEdit3(kDiffuse[current_language_], (float*)&material.Diffuse);
		ImGui::ColorEdit3(kAmbient[current_language_], (float*)&material.Ambient);
		ImGui::ColorEdit3(kSpecular[current_language_], (float*)&material.Specular);
		ImGui::ColorEdit3(kEmissive[current_language_], (float*)&material.Emissive);
		ImGui::InputFloat(kPower[current_language_], &material.Power);
		ImGui::Text(kColorTextureName[current_language_], material.ColorTexture.c_str());
		if (ImGui::Button(kChangeColorTexture[current_language_]))
		{
			ChangeTexture(material.ColorTexture);
		}
		ImGui::Text(kDiffuseTextureName[current_language_], material.DiffuseTexture.c_str());
		if (ImGui::Button(kChangeDiffuseTexture[current_language_]))
		{
			ChangeTexture(material.DiffuseTexture);
		}
		ImGui::Text(kDiffuseMaskName[current_language_], material.DiffuseTextureMask.c_str());
		if (ImGui::Button(kChangeDiffuseMask[current_language_]))
		{
			ChangeTexture(material.DiffuseTextureMask);
		}
		ImGui::Text(kSpecularTextureName[current_language_], material.SpecularTexture.c_str());
		if (ImGui::Button(kChangeSpecularTexture[current_language_]))
		{
			ChangeTexture(material.SpecularTexture);
		}
		ImGui::Text(kSpecularMaskName[current_language_], material.SpecularTextureMask.c_str());
		if (ImGui::Button(kChangeSpecularMask[current_language_]))
		{
			ChangeTexture(material.SpecularTextureMask);
		}
		ImGui::Text(kNormalTextureName[current_language_], material.NormalTexture.c_str());
		if (ImGui::Button(kChangeNormalTexture[current_language_]))
		{
			ChangeTexture(material.NormalTexture);
		}
		ImGui::Text(kDetailTextureName[current_language_], material.DetailTexture.c_str());
		if (ImGui::Button(kChangeDetailTexture[current_language_]))
		{
			ChangeTexture(material.DetailTexture);
		}
		ImGui::Text(kDetailMaskName[current_language_], material.DetailMask.c_str());
		if (ImGui::Button(kChangeDetailMask[current_language_]))
		{
			ChangeTexture(material.DetailMask);
		}
		ImGui::Text(kTintByBaseMaskName[current_language_], material.TintByBaseMask.c_str());
		if (ImGui::Button(kChangeTintByBaseMask[current_language_]))
		{
			ChangeTexture(material.TintByBaseMask);
		}
		ImGui::Text(kRimMaskName[current_language_], material.RimMask.c_str());
		if (ImGui::Button(kChangeRimMask[current_language_]))
		{
			ChangeTexture(material.RimMask);
		}
		ImGui::Text(kTranslucency[current_language_], material.Translucency.c_str());
		if (ImGui::Button(kChangeTranslucency[current_language_]))
		{
			ChangeTexture(material.Translucency);
		}
		ImGui::Text(kMetalnessMask[current_language_], material.MetalnessMask.c_str());
		if (ImGui::Button(kChangeMetalnessMask[current_language_]))
		{
			ChangeTexture(material.MetalnessMask);
		}
		ImGui::Text(kSelfIllumMask[current_language_], material.SelfIllumMask.c_str());
		if (ImGui::Button(kChangeSelfIllumMask[current_language_]))
		{
			ChangeTexture(material.SelfIllumMask);
		}
		ImGui::Text(kFresnelWarpColor[current_language_], material.FresnelWarpColor.c_str());
		if (ImGui::Button(kChangeFresnelWarpColor[current_language_]))
		{
			ChangeTexture(material.FresnelWarpColor);
		}
		ImGui::Text(kFresnelWarpRim[current_language_], material.FresnelWarpRim.c_str());
		if (ImGui::Button(kChangeFresnelWarpRim[current_language_]))
		{
			ChangeTexture(material.FresnelWarpRim);
		}
		ImGui::Text(kFresnelWarpSpecular[current_language_], material.FresnelWarpSpecular.c_str());
		if (ImGui::Button(kChangeFresnelWarpSpecular[current_language_]))
		{
			ChangeTexture(material.FresnelWarpSpecular);
		}

		if (ImGui::Button(kAdd[current_language_]))
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
			material.DetailTexture.clear();
			material.DetailMask.clear();
			material.TintByBaseMask.clear();
			material.RimMask.clear();
			material.Translucency.clear();
			material.MetalnessMask.clear();
			material.SelfIllumMask.clear();
			material.FresnelWarpColor.clear();
			material.FresnelWarpRim.clear();
			material.FresnelWarpSpecular.clear();
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

	auto camera = CMain::GetManager()->GetMode()->GetCamera();
	auto position_at = camera->GetPosAt();
	auto position_eye = camera->GetPosEye();

	// PosAt
	if (ImGui::InputFloat3(kPositionAt[current_language_], &position_at.m_fX))
	{
		camera->SetPosAt(position_at);
	}
	
	// PosEye
	if (ImGui::InputFloat3(kPositionEye[current_language_], &position_eye.m_fX))
	{
		camera->SetPosEye(position_eye);
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
	bool is_delete = false;
	if (ImGui::CollapsingHeader(kCurrentAnimation[current_language_]))
	{
		auto& current = animator_->Motions[motion_no_];
		
		// 名前
		char buffer[kBufferSize] = {};
		strcpy_s(buffer, current.Name.c_str());
		if (ImGui::InputText(kName[current_language_], buffer, kBufferSize))
		{
			current.Name = buffer;
		}

		// フレーム編集
		if (ImGui::TreeNode(kEditFrame[current_language_]))
		{
			// Start Frame
			if (ImGui::SliderInt(kStartFrame[current_language_], &current.StartFrame, 0, current.EndFrame))
			{
				if (current_frame_ < current.StartFrame)
				{
					current_frame_ = current.StartFrame;
				}
			}

			// End Frame
			if (ImGui::SliderInt(kEndFrame[current_language_], &current.EndFrame, current.StartFrame, (int)current.Frames.size() - 1))
			{
				if (current_frame_ > current.EndFrame)
				{
					current_frame_ = current.EndFrame;
				}
			}

			// Delete Frame that out of range
			if (ImGui::Button(kDeleteOutOfRangeFrames[current_language_]))
			{
				animator_->DeleteOutOfRangeFrames(motion_no_);
				current_frame_ = current.StartFrame;
			}

			ImGui::TreePop();
		}

		// モーション切り替え設定
		if (ImGui::TreeNode(kStateTransition[current_language_]))
		{
			ImGui::Checkbox(kIsLoop[current_language_], &current.IsLoop);

			ImGui::Checkbox(kTransitionWhenIsOver[current_language_], &current.ChangeWhenOver);
			ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), kTransitionNotice[current_language_]);

			if (!current.IsLoop && current.ChangeWhenOver)
			{
				ImGui::SliderInt(kOverFrame[current_language_], &current.ChangeWhenOverExitFrame, current.StartFrame, current.EndFrame);
				ImGui::InputInt(kBlendFrameNumber[current_language_], &current.ChangeWhenOverBlendFrame);
				char name_buffer[kBufferSize] = {};
				strcpy_s(name_buffer, current.ChangeWhenOverNextMotion.c_str());
				if (ImGui::InputText(kNextAnimationName[current_language_], name_buffer, kBufferSize))
				{
					current.ChangeWhenOverNextMotion = name_buffer;
				}
			}

			int transition_counter = 0;
			for (auto iterator = current.Transitions.begin(); iterator != current.Transitions.end();)
			{
				bool is_delete_transition = false;
				string& tree_name = kTransition[current_language_] + std::to_string(transition_counter);
				if (ImGui::TreeNode(tree_name.c_str()))
				{
					// 次のモーション名
					char name_buffer[kBufferSize] = {};
					strcpy_s(name_buffer, iterator->NextMotion.c_str());
					if (ImGui::InputText(kNextAnimationName[current_language_], name_buffer, kBufferSize))
					{
						iterator->NextMotion = name_buffer;
					}

					// Blend frame
					ImGui::InputInt(kBlendFrameNumber[current_language_], &iterator->BlendFrame);

					// 条件
					int condition_counter = 0;
					for (auto itrCondition = iterator->Conditions.begin(); itrCondition != iterator->Conditions.end();)
					{
						bool is_delete_condition = false;
						string& conditiontree_name = kCondition[current_language_] + std::to_string(condition_counter);
						if (ImGui::TreeNode(conditiontree_name.c_str()))
						{
							// 条件変数の型
							static const char* type_labels[] =
							{
								"bool",
								"float",
							};
							ImGui::Combo(kParameterType[current_language_], (int*)&itrCondition->ParameterType, type_labels, 2);

							// 条件変数名
							char parametername_buffer[kBufferSize] = {};
							strcpy_s(parametername_buffer, itrCondition->ParameterName.c_str());
							if (ImGui::InputText(kParameterName[current_language_], parametername_buffer, kBufferSize))
							{
								itrCondition->ParameterName = parametername_buffer;
							}

							// オペレーター
							if (itrCondition->ParameterType == eParameterType::eFloat)
							{
								static const char* operator_labels[] =
								{
									"equal",
									"not equal",
									"greater",
									"less"
								};
								ImGui::Combo(kOperator[current_language_], (int*)&itrCondition->FloatOperator, operator_labels, 4);
							}

							// 値
							switch (itrCondition->ParameterType)
							{
							case eParameterType::eBool:
							{
								static const char* value_labels[] =
								{
									"is false",
									"is true",
								};
								ImGui::Combo(kValue[current_language_], (int*)&itrCondition->BoolValue, value_labels, 2);
								break;
							}
							case eParameterType::eFloat:
							{
								ImGui::InputFloat(kValue[current_language_], &itrCondition->FloatValue);
								break;
							}
							}

							// 条件の削除
							if (ImGui::Button(kDeleteCondition[current_language_]))
							{
								is_delete_condition = true;
							}
							ImGui::TreePop();
						}

						// Count iterator
						if (is_delete_condition)
						{
							itrCondition = iterator->Conditions.erase(itrCondition);
						}
						else
						{
							++condition_counter;
							++itrCondition;
						}
					}

					// 条件の追加
					if (ImGui::Button(kAddCondition[current_language_]))
					{
						iterator->Conditions.push_back(Condition());
					}

					// 切り替えの削除
					if (ImGui::Button(kDeleteTransition[current_language_]))
					{
						is_delete_transition = true;
					}
					ImGui::TreePop();
				}

				// Count iterator
				if (is_delete_transition)
				{
					iterator = current.Transitions.erase(iterator);
				}
				else
				{
					++transition_counter;
					++iterator;
				}
			}

			// 切り替えの追加
			if (ImGui::Button(kAddTransition[current_language_]))
			{
				current.Transitions.push_back(StateTransition());
			}

			// Save Transition
			if (ImGui::Button(kSaveTransition[current_language_]))
			{
				animator_->SaveMotionTransitions(motion_no_);
			}

			ImGui::TreePop();
		}

		// Delete Animation
		if (ImGui::Button(kDeleteAnimation[current_language_]))
		{
			is_delete ^= 1;
		}
	}
	if (is_delete)
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
	string texture_name;
	if (CMain::OpenTextureFile(texture_name))
	{
		string name, type;
		CKFUtility::AnalyzeFilePath(texture_name, name, type);
		if (CKFUtility::CheckIsTexture(type))
		{
			if (!meshTexture.empty())
			{
				CMain::GetManager()->GetTextureManager()->DisuseTexture(meshTexture);
			}
			meshTexture = name + '.' + type;
			CMain::GetManager()->GetTextureManager()->UseTexture(meshTexture);
		}
	}
}

//--------------------------------------------------------------------------------
// AddAnimation
//--------------------------------------------------------------------------------
void CModelAnalyzerBehaviorComponent::AddAnimation(void)
{
	string file_name;
	if (CMain::OpenTextureFile(file_name))
	{
		string name, type;
		CKFUtility::AnalyzeFilePath(file_name, name, type);
		if (type._Equal("fbx") || type._Equal("FBX"))
		{
			CKFUtilityFBX::LoadAnimation(file_name, animator_);
		}
	}
}

//--------------------------------------------------------------------------------
// ShowChangeParent
//--------------------------------------------------------------------------------
bool CModelAnalyzerBehaviorComponent::ChangeParent(CMyNode* pNode)
{
	int index = 0;
	if (ImGui::Combo(kChangeParent[current_language_], (int*)&index, node_names_))
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