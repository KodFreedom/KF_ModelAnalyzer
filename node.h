//--------------------------------------------------------------------------------
//
//　node.h
//	Author : Xu Wenjie
//	Date   : 2017-09-15
//--------------------------------------------------------------------------------
#pragma once

//--------------------------------------------------------------------------------
//  インクルードファイル
//--------------------------------------------------------------------------------
#include "main.h"
#include "KF_CollisionSystem.h"
#include "animator.h"

//--------------------------------------------------------------------------------
//  構造体定義
//--------------------------------------------------------------------------------
struct BoneReference
{
	BoneReference(const us index, float weight, const string& name)
		: Index(index), Weight(weight), Name(name) {}
	us		Index;
	string	Name;
	float	Weight;
};

struct ControlPoint
{
	ControlPoint() {}
	ControlPoint(const CKFVec3& position) : Position(position) {}
	CKFVec3					Position;
	vector<BoneReference>	BoneReferences;
};

struct Texture
{
	string	UVSetName;
	string	Name;
};

struct Lambert
{
	CKFVec4			Ambient;
	CKFVec4			Diffuse;
	CKFVec4			Emissive;
	CKFVec3			Bump;
	float			Transparency;
};

struct Phong
{
	CKFVec4			Specular;
	float			Shininess;
	float			Reflectivity;
};

struct UVSet
{
	string			Name;
	vector<CKFVec2>	UVs;
	vector<us>		UVIndeces;
};

#ifdef USING_DIRECTX
struct VertexDX
{
	VERTEX_3D				Vertex;
	vector<BoneReference>	BoneReferences;
	bool operator==(const VertexDX& vValue) const;
};
#endif

struct Mesh
{
	Mesh()
		: MaterialIndex(-1)
		, EnableCullFace(false)
		, EnableLight(false)
		, EnableFog(false)
		, Diffuse(CKFColor(1.0f))
		, Ambient(CKFColor(1.0f))
		, Specular(CKFColor(1.0f))
		, Emissive(CKFColor(1.0f))
		, Power(1.0f)
		, RenderPriority(RP_3D)
#ifdef USING_DIRECTX
		, VertexNumber(0)
		, IndexNumber(0)
		, PolygonNumber(0)
		, VertexBuffer(nullptr)
		, IndexBuffer(nullptr)
#endif
	{}

	vector<ControlPoint>	Points;
	vector<CKFVec3>			Normals;
	vector<UVSet>			UVSets;
	vector<us>				PointIndeces;
	vector<us>				NormalIndeces;
	short					MaterialIndex;
	string					DiffuseTextureName;
	string					SpecularTextureName;
	string					NormalTextureName;
	CKFColor				Ambient;	// 環境光の反射率
	CKFColor				Diffuse;	// 漫射光の反射率
	CKFColor				Specular;	// 鏡面光の反射率
	CKFColor				Emissive;	// 自発光
	float					Power;		// ハイライトのシャープネス
	bool					EnableCullFace;
	bool					EnableLight;
	bool					EnableFog;
	RENDER_PRIORITY			RenderPriority;

#ifdef USING_DIRECTX
	int						VertexNumber;
	int						IndexNumber;
	int						PolygonNumber;
	vector<VertexDX>		Verteces;
	LPDIRECT3DVERTEXBUFFER9 VertexBuffer;
	LPDIRECT3DINDEXBUFFER9	IndexBuffer;
#endif
};

struct ColliderInfo
{//コライダー
	CS::COL_TYPE	Type;
	CKFVec3			Position;
	CKFVec3			Rotation;
	CKFVec3			Scale;
};

//--------------------------------------------------------------------------------
//  クラス定義
//--------------------------------------------------------------------------------
class CMyNode
{
	friend class CKFUtilityFBX;

public:
	CMyNode()
		: Translation(CKFVec3(0.0f))
		, Rotation(CKFVec3(0.0f))
		, Scale(CKFVec3(1.0f))
		, ColliderMaterialID(1)
	{}
	~CMyNode() {}

	list<CMyNode*>	Children;
	string			Name;
	vector<string>	AttributeNames;

	// Offset
	CKFVec3			Translation;
	CKFVec3			Rotation;
	CKFVec3			Scale;
	
	// Matrix
	CKFMtx44		Local;
	CKFMtx44		World;

	vector<Texture>	Textures;
	vector<Mesh>	Meshes;

	//Collider
	list<ColliderInfo>	Colliders;
	us					ColliderMaterialID;		//Collider Mat

	void Release(void);
	void RecursiveUpdateMatrix(const CKFMtx44& parent);
	void RecursiveUpdateSkin(const vector<Cluster>& clusters);
	void RecursiveDraw(const bool& drawSkeleton, const bool& drawMesh, const bool& drawCollider);
	void RecursiveRecombineMeshes(void);
	void RecursiveReverseTexV(void);
	void RecalculateMeshesBy(const CKFMtx44& matrix);
	void RecursiveMatchClusterID(const Frame& initFrame);

private:
	void analyzePoint(FbxMesh* pMesh);
	void analyzeNormal(FbxMesh* pMesh);
	void analyzeUV(FbxMesh* pMesh);
	void analyzeTexture(FbxNode* pNode);
	void analyzeMaterial(FbxMesh* pMesh);
	void analyzeCluster(FbxMesh* pMesh);

#ifdef USING_DIRECTX
	static LPD3DXMESH s_pMeshSphere;
	static LPD3DXMESH s_pMeshCube;
#endif
};