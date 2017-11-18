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
#include "drawComponent.h"

//--------------------------------------------------------------------------------
//  前方宣言
//--------------------------------------------------------------------------------
class CMyNode;
class CAnimator;

//--------------------------------------------------------------------------------
//  構造体定義
//--------------------------------------------------------------------------------
struct BornRefarence
{
	BornRefarence(short sIndex, float fWeight, const string& Name)
		: sIndex(sIndex), fWeight(fWeight), strBoneName(Name) {}
	short			sIndex;
	string			strBoneName;	//保存用
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
	string					Name;
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
		, m_bEnableFog(false)
		, Diffuse(CKFColor(1.0f))
		, Ambient(CKFColor(1.0f))
		, Specular(CKFColor(1.0f))
		, Emissive(CKFColor(1.0f))
		, Power(1.0f)
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
	CKFColor				Ambient;	// 環境光の反射率
	CKFColor				Diffuse;	// 漫射光の反射率
	CKFColor				Specular;	// 鏡面光の反射率
	CKFColor				Emissive;	// 自発光
	float					Power;		// ハイライトのシャープネス
	bool					m_bEnableCullFace;
	bool					m_bEnableLight;
	bool					m_bEnableFog;
	RENDER_PRIORITY			m_renderPriority; 

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

struct BoneFrame
{
	string		Name;
	CKFMtx44	Matrix;
};

struct Frame
{
	Frame() {}
	~Frame() { BoneFrames.clear(); BoneFrames.shrink_to_fit(); }
	vector<BoneFrame> BoneFrames;
};

struct Motion
{
	Motion() : StartFrame(0), EndFrame(0)
	{
		Name.clear();
		Frames.clear();
	}
	string			Name;
	vector<Frame>	Frames;
	int				StartFrame;
	int				EndFrame;
};

struct MyModel
{
	MyModel() : pNode(nullptr), pAnimator(nullptr){}
	CMyNode*	pNode;
	CAnimator*	pAnimator;
};

struct Cluster
{
	Cluster() : Node(nullptr) {}
	string		Name;
	CMyNode*	Node;
	CKFMtx44	RelativeInitPosition;
	CKFMtx44	World;
};

//--------------------------------------------------------------------------------
//  クラス定義
//--------------------------------------------------------------------------------
class CAnimator
{
public:
	CAnimator() {}
	~CAnimator() {}

	void Release(void)
	{
		for (auto& motion : Motions)
		{
			for (auto& frame : motion.Frames)
			{
				for (auto& boneFrame : frame.BoneFrames) { boneFrame.Name.clear(); }
				frame.BoneFrames.clear();
			}
			motion.Frames.clear();
		}
		Motions.clear();
	}

	vector<Motion> Motions;
	vector<Cluster> Clusters;
	void UpdateBones(const Frame& current);
	void UpdateClusterWorld(void);
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
		Name.clear();
		vecAttributeName.clear();
		vecTex.clear();
		vecMesh.clear();
		listCollider.clear();
	}
	~CMyNode() {}

	list<CMyNode*>	listChild;
	string			Name;
	vector<string>	vecAttributeName;

	//Offset
	CKFVec3			vTrans;
	CKFVec3			vRot;
	CKFVec3			vScale;
	CKFMtx44		local;
	CKFMtx44		world;

	vector<Texture>	vecTex;
	vector<Mesh>	vecMesh;

	//Collider
	list<COL_INFO>	listCollider;
	unsigned short	materialID;		//Collider Mat

	void Release(void);
	void RecursiveUpdateMatrix(const CKFMtx44& parent);
	void RecursiveUpdateSkin(const vector<Cluster>& clusters);
	void RecursiveDraw(const bool& drawSkeleton, const bool& drawMesh, const bool& drawCollider);
	void RecursiveRecalculateVtx(void);
	void RecursiveReverseTexV(void);
	void RecalculateVtxByMatrix(const CKFMtx44& mtx);
	void RecursiveRecalculateClusterID(const Frame& initFrame);

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
	static MyModel	LoadFromTxt(const string& strFilePath);
	static bool		Save(CMyNode* pRootNode, const string& strFileName);
	static void		LoadAnimation(const string& strFilePath, CAnimator* animator);

#ifdef USING_DIRECTX
	static int		FindRepetition(const list<VtxDX>& listVtx, const VtxDX& vtx);
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