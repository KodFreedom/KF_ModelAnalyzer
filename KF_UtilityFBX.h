//--------------------------------------------------------------------------------
//
//�@KF_UtilityFBX.h
//	Author : Xu Wenjie
//	Date   : 2017-09-15
//--------------------------------------------------------------------------------
#pragma once

//--------------------------------------------------------------------------------
//  �C���N���[�h�t�@�C��
//--------------------------------------------------------------------------------
#include "main.h"
#include "KF_CollisionSystem.h"
#include "drawComponent.h"
#include "node.h"
#include "animator.h"
#include "material.h"

//--------------------------------------------------------------------------------
//  �񋓌^
//--------------------------------------------------------------------------------
enum OutType
{
	Json,
	Binary
};

//--------------------------------------------------------------------------------
//  �\���̒�`
//--------------------------------------------------------------------------------
struct MyModel
{
	MyModel() : pNode(nullptr), pAnimator(nullptr) {}
	CMyNode*						pNode;
	CAnimator*						pAnimator;
	unordered_map<string, Material>	mapMaterial;
};

//--------------------------------------------------------------------------------
//  �N���X��`
//--------------------------------------------------------------------------------
class CKFUtilityFBX
{
public:
	static MyModel	Load(const string& strFilePath);
	static MyModel	LoadFromTxt(const string& strFilePath);
	static bool		Save(const MyModel& model, const string& fileName, const OutType& type);
	static void		LoadAnimation(const string& strFilePath, CAnimator* animator);

#ifdef USING_DIRECTX
	static int		FindRepetition(const list<VertexDX>& listVtx, const VertexDX& vtx);
#endif
private:
	CKFUtilityFBX() {}
	~CKFUtilityFBX() {}

	static CMyNode*		recursiveNode(FbxManager* pManager, FbxNode* pNode);
	static void			analyzeMaterial(FbxScene* lScene, unordered_map<string, Material>& mapMaterial);
	static CAnimator*	analyzeAnimator(FbxImporter* lImporter, FbxScene* lScene);
	static void			analyzeAnimation(FbxImporter* lImporter, FbxScene* lScene, CAnimator* animator);
	static void			findSkeletons(FbxNode* pNode, list<FbxNode*>& listSkeleton);
	static void			matchClusterWithSkeleton(vector<Cluster>& clusters, CMyNode* node);
	static FbxAMatrix	getGeometry(FbxNode* pNode);
	static string		getAttributeTypeName(FbxNodeAttribute::EType type);
};