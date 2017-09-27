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

//--------------------------------------------------------------------------------
//  �O���錾
//--------------------------------------------------------------------------------
class CMyNode;
class CAnimator;

//--------------------------------------------------------------------------------
//  �\���̒�`
//--------------------------------------------------------------------------------
struct BornRefarence
{
	BornRefarence(unsigned char ucIndex, float fWeight, const string& strName)
		: ucIndex(ucIndex), fWeight(fWeight), strBoneName(strName) {}
	unsigned char	ucIndex;
	string			strBoneName;	//�ۑ��p
	float			fWeight;
};

struct ControlPoint
{
	ControlPoint() {}
	ControlPoint(const CKFVec3& vPos) : vPos(vPos) {}
	CKFVec3					vPos;
	vector<BornRefarence>	vecBornRefarence;
};

struct Lambert
{
	CKFVec4			vAmbient;
	CKFVec4			vDiffuse;
	CKFVec4			vEmissive;
	CKFVec3			vBump;
	float			fTransparency;
};

struct Phong
{
	CKFVec4			vSpecular;
	float			fShininess;
	float			fReflectivity;
};

struct Texture
{
	string					strUVSetName;
	string					strName;
};

struct UVSET
{
	string					strUVSetName;
	vector<CKFVec2>			vecUV;
	vector<unsigned short>	vecUVIdx;
};

#ifdef USING_DIRECTX
struct VtxDX
{
	VERTEX_3D				vtx;
	vector<BornRefarence>	vecBornRefarence;

	bool operator==(const VtxDX& vValue) const;
};
#endif

struct Mesh
{
	Mesh()
		: nMaterialIndex(-1)
		, m_bEnableCullFace(false)
		, m_bEnableLight(false)
		, m_renderPriority(RP_3D)
#ifdef USING_DIRECTX
		, m_nNumVtx(0)
		, m_nNumIdx(0)
		, m_nNumPolygon(0)
		, m_pVtxBuffer(nullptr)
		, m_pIdxBuffer(nullptr)
#endif
	{
		vecPoint.clear();
		vecNormal.clear();
		vecUVSet.clear();
		vecPoint.clear();
		vecNormalIdx.clear();
		strTexName.clear();

#ifdef USING_DIRECTX
		m_vecVtx.clear();
#endif
	}

	vector<ControlPoint>	vecPoint;
	vector<CKFVec3>			vecNormal;
	vector<UVSET>			vecUVSet;
	vector<unsigned short>	vecPointIdx;
	vector<unsigned short>	vecNormalIdx;
	int						nMaterialIndex;	//Texture
	string					strTexName;
	bool					m_bEnableCullFace;
	bool					m_bEnableLight;
	RENDER_PRIORITY			m_renderPriority; 

#ifdef USING_DIRECTX
	int						m_nNumVtx;
	int						m_nNumIdx;
	int						m_nNumPolygon;
	vector<VtxDX>			m_vecVtx;
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuffer;	// ���_�o�b�t�@�ւ̃|�C���^
	LPDIRECT3DINDEXBUFFER9	m_pIdxBuffer;	// �C���f�b�N�X�ւ̃|�C���^
#endif
};

struct COL_INFO
{//�R���C�_�[
	CS::COL_TYPE	colType;
	CKFVec3			vOffsetPos;
	CKFVec3			vOffsetRot;
	CKFVec3			vOffsetScale;
};

struct Cluster
{
	string		strName;
	CKFMtx44	mtx;
};

struct Avatar
{
	vector<Cluster> vecCluster;
};

struct Motion
{
	string			strName;
	vector<Avatar>	vecAvator;
};

struct MyModel
{
	MyModel() : pNode(nullptr), pAnimator(nullptr){}
	CMyNode*	pNode;
	CAnimator*	pAnimator;
};

//--------------------------------------------------------------------------------
//  �N���X��`
//--------------------------------------------------------------------------------
class CAnimator
{
public:
	CAnimator() {}
	~CAnimator() {}

	void Release(void)
	{
		for (auto& motion : m_vecMotion)
		{
			for (auto& avatar : motion.vecAvator)
			{
				for (auto& cluster : avatar.vecCluster) { cluster.strName.clear(); }
				avatar.vecCluster.clear();
			}
			motion.vecAvator.clear();
		}
		m_vecMotion.clear();
	}

	vector<Motion> m_vecMotion;
};

class CMyNode
{
	friend class CKFUtilityFBX;

public:
	CMyNode()
		: vTrans(CKFVec3(0.0f))
		, vRot(CKFVec3(0.0f))
		, vScale(CKFVec3(0.0f))
		, materialID(1)
	{
		listChild.clear();
		strName.clear();
		vecAttributeName.clear();
		vecTex.clear();
		vecMesh.clear();
		listCollider.clear();
	}
	~CMyNode() {}

	list<CMyNode*>	listChild;
	string			strName;
	vector<string>	vecAttributeName;

	//Offset
	CKFVec3			vTrans;
	CKFVec3			vRot;
	CKFVec3			vScale;

	vector<Texture>	vecTex;
	vector<Mesh>	vecMesh;

	//Collider
	list<COL_INFO>	listCollider;
	unsigned short	materialID;		//Collider Mat

	void Release(void);
	void RecursiveUpdate(const Avatar& avatar);
	void RecursiveDraw(const bool& bDrawNormal, const CKFMtx44& mtxParent);
	void RecursiveRecalculateVtx(void);
	void RecursiveReverseTexV(void);
	void RecalculateVtxByMatrix(const CKFMtx44& mtx);

private:
	void		analyzePos(FbxMesh* pMesh);
	void		analyzeNormal(FbxMesh* pMesh);
	void		analyzeUV(FbxMesh* pMesh);
	void		analyzeTexture(FbxNode* pNode);
	void		analyzeMaterial(FbxMesh* pMesh);
	void		analyzeCluster(FbxMesh* pMesh);
	//void		analyzeSkeleton(FbxSkeleton* pSkeleton);
	
#ifdef USING_DIRECTX
	static LPD3DXMESH s_pMeshSphere;
	static LPD3DXMESH s_pMeshCube;
#endif
};

class CKFUtilityFBX
{
public:
	static MyModel	Load(const string& strFilePath);
	static bool		Save(CMyNode* pRootNode, const string& strFileName);

#ifdef USING_DIRECTX
	static int		FindRepetition(const list<VtxDX>& listVtx, const VtxDX& vtx);
#endif
private:
	CKFUtilityFBX() {}
	~CKFUtilityFBX() {}

	static CMyNode*		recursiveNode(FbxManager* pManager, FbxNode* pNode);
	static CAnimator*	analyzeAnimation(FbxImporter* lImporter, FbxScene* lScene);
	static void			analyzePose(FbxScene* lScene);
	static FbxMesh*		findMeshNode(FbxNode* pNode);
	static void			recursiveSaveNode(FILE* pFile, CMyNode* pNode, const string& strFileName);
	static void			saveMesh(const CMyNode* pNode, const Mesh& mesh, const string& strMeshName);
	static void			saveOneSkinMesh(const CMyNode* pNode, const Mesh& mesh, const string& strMeshName);
	static FbxAMatrix	getGeometry(FbxNode* pNode);
	static string		getAttributeTypeName(FbxNodeAttribute::EType type);

};