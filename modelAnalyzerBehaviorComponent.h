//--------------------------------------------------------------------------------
//	モデルアナライザービヘイビアコンポネント
//　modelAnalyzerBehaviorComponent.h
//	Author : Xu Wenjie
//	Date   : 2017-07-19
//--------------------------------------------------------------------------------
#pragma once

//--------------------------------------------------------------------------------
//  インクルードファイル
//--------------------------------------------------------------------------------
#include "behaviorComponent.h"
#include "KF_UtilityFBX.h"
#include "material.h"

//--------------------------------------------------------------------------------
//  前方宣言
//--------------------------------------------------------------------------------
class CMyNode;
class CAnimator;

//--------------------------------------------------------------------------------
//  クラス宣言
//--------------------------------------------------------------------------------
class CModelAnalyzerBehaviorComponent : public CBehaviorComponent
{
public:
	//--------------------------------------------------------------------------------
	//  関数定義
	//--------------------------------------------------------------------------------
	CModelAnalyzerBehaviorComponent(CGameObject* const pGameObj);
	~CModelAnalyzerBehaviorComponent() {}

	bool Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void LateUpdate(void) override;

	void OnTrigger(CColliderComponent& colliderThis, CColliderComponent& collider) override {}
	void OnCollision(CCollisionInfo& collisionInfo) override {}

	auto GetRootNode(void) const { return root_node_; }
	void ChangeModel(const string& file_path);
	void SaveModel(const OutType& type);

	const bool& IsRenderSkeletons(void) const { return is_render_skeletons_; }
	const bool& IsRenderMeshes(void) const { return is_render_meshes_; }
	const bool& IsRenderColliders(void) const { return is_render_colliders_; }
	const auto& GetMaterials(void) const { return materials_; }

private:
	//--------------------------------------------------------------------------------
	//  定数定義
	//--------------------------------------------------------------------------------
	static constexpr int kBufferSize = 64;

	//--------------------------------------------------------------------------------
	//  関数定義
	//--------------------------------------------------------------------------------
	void		ReleaseModel(void);

	//ImGui
	void		ShowMainMenuBar(void);
	void		ShowMainMenuFile(void);
	void		ShowMainWindow(void);
	void		ShowModelInfoWindow(void);
	CMyNode*	ShowNodeInfo(CMyNode* pNode);
	void		ShowNodeNowWindow(void);
	void		ShowAnimatorWindow(void);
	void		ShowCameraWindow(void);
	void		ShowMaterialWindow(void);
	void		ShowCurrentAnimationWindow(void);
	bool		ChangeParent(CMyNode* pNode);
	void		ChangeTexture(string& meshTexture);
	void		AddAnimation(void);

	//--------------------------------------------------------------------------------
	//  変数定義
	//--------------------------------------------------------------------------------
	bool		     is_render_skeletons_;
	bool		     is_render_meshes_;
	bool		     is_render_colliders_;
	bool		     is_reserve_texcoordv_;
	bool		     is_saved_;
	string		     file_name_;
	CMyNode*	     root_node_;
	CAnimator*	     animator_;
	vector<string>   node_names_;
	vector<CMyNode*> nodes_;
	unordered_map<string, Material>	materials_;

	//ImGui
	float		rotation_speed_;
	bool		is_display_model_window_;
	bool		is_display_animator_window_;
	bool		is_display_camera_window_;
	bool		is_display_material_window_;
	CMyNode*	current_node_;
	CKFVec3		current_node_correct_translation_;
	CKFVec3		current_node_correct_rotation_;
	CKFVec3		current_node_correct_scale_;

	//Animator
	bool		is_playing_motion_;
	int			current_frame_;
	int			motion_no_;
};