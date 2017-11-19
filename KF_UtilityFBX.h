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

//--------------------------------------------------------------------------------
//  �\���̒�`
//--------------------------------------------------------------------------------
struct MyModel
{
	MyModel() : pNode(nullptr), pAnimator(nullptr) {}
	CMyNode*	pNode;
	CAnimator*	pAnimator;
};

//--------------------------------------------------------------------------------
//  �N���X��`
//--------------------------------------------------------------------------------
class CKFUtilityFBX
{
public:
	static MyModel	Load(const string& strFilePath);
	static MyModel	LoadFromTxt(const string& strFilePath);
	static bool		Save(CMyNode* pRootNode, const string& strFileName);
	static void		LoadAnimation(const string& strFilePath, CAnimator* animator);

#ifdef USING_DIRECTX
	static int		FindRepetition(const list<VertexDX>& listVtx, const VertexDX& vtx);
#endif
private:
	CKFUtilityFBX() {}
	~CKFUtilityFBX() {}

	static CMyNode*		recursiveNode(FbxManager* pManager, FbxNode* pNode);
	static CAnimator*	analyzeAnimation(FbxImporter* lImporter, FbxScene* lScene);
	static void			analyzeAnimation(FbxImporter* lImporter, FbxScene* lScene, CAnimator* animator);
	static void			analyzePose(FbxScene* lScene);
	static void			findSkeletons(FbxNode* pNode, list<FbxNode*>& listSkeleton);
	static void			matchClusterWithSkeleton(vector<Cluster>& clusters, CMyNode* node);
	static void			recursiveSaveNode(FILE* pFile, CMyNode* pNode, const string& strFileName);
	static void			saveMesh(const CMyNode* pNode, const Mesh& mesh, const string& strMeshName);
	static void			saveOneSkinMesh(const CMyNode* pNode, const Mesh& mesh, const string& strMeshName);
	static FbxAMatrix	getGeometry(FbxNode* pNode);
	static string		getAttributeTypeName(FbxNodeAttribute::EType type);
};