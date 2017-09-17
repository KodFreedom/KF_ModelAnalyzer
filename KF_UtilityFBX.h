//--------------------------------------------------------------------------------
//
//　KF_UtilityFBX.h
//	Author : Xu Wenjie
//	Date   : 2017-09-15
//--------------------------------------------------------------------------------
#pragma once

//--------------------------------------------------------------------------------
//  インクルードファイル
//--------------------------------------------------------------------------------
#include "main.h"
#include "KF_CollisionSystem.h"

//--------------------------------------------------------------------------------
//  構造体定義
//--------------------------------------------------------------------------------
struct BornRefarence
{
	BornRefarence(unsigned char ucIndex, float fWeight) : ucIndex(ucIndex), fWeight(fWeight) {}
	unsigned char	ucIndex;
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
		vecMtx.clear();
#ifdef USING_DIRECTX
		m_vecVtx.clear();
#endif
	}

	vector<ControlPoint>	vecPoint;
	vector<CKFVec3>			vecNormal;
	vector<UVSET>			vecUVSet;
	vector<unsigned short>	vecPointIdx;
	vector<unsigned short>	vecNormalIdx;
	int						nMaterialIndex;
	vector<CKFMtx44>		vecMtx;

#ifdef USING_DIRECTX
	int						m_nNumVtx;
	int						m_nNumIdx;
	int						m_nNumPolygon;
	vector<VtxDX>			m_vecVtx;
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuffer;	// 頂点バッファへのポインタ
	LPDIRECT3DINDEXBUFFER9	m_pIdxBuffer;	// インデックスへのポインタ
#endif
};

struct COL_INFO
{//コライダー
	CS::COL_TYPE	colType;
	CKFVec3			vOffsetPos;
	CKFVec3			vOffsetRot;
	CKFVec3			vOffsetScale;
};

//--------------------------------------------------------------------------------
//  クラス定義
//--------------------------------------------------------------------------------
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
	void RecursiveUpdate(void);
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
	FbxAMatrix	getGeometry(FbxNode* pNode);

#ifdef USING_DIRECTX
	static LPD3DXMESH s_pMeshSphere;
	static LPD3DXMESH s_pMeshCube;
#endif
};

class CKFUtilityFBX
{
public:
	static CMyNode*	Load(const string& strFilePath);
	
#ifdef USING_DIRECTX
	static int		FindRepetition(const list<VtxDX>& listVtx, const VtxDX& vtx);
#endif
private:
	CKFUtilityFBX() {}
	~CKFUtilityFBX() {}

	static CMyNode*	recursiveNode(FbxManager* pManager, FbxNode* pNode);
	static string	getAttributeTypeName(FbxNodeAttribute::EType type);
};