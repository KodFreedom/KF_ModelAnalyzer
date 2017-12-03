//--------------------------------------------------------------------------------
//
//�@node.h
//	Author : Xu Wenjie
//	Date   : 2017-09-15
//--------------------------------------------------------------------------------
#pragma once

//--------------------------------------------------------------------------------
//  �C���N���[�h�t�@�C��
//--------------------------------------------------------------------------------
#include "main.h"
#include "KF_CollisionSystem.h"
#include "animator.h"
#include "material.h"

//--------------------------------------------------------------------------------
//  �O���錾
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//  �\���̒�`
//--------------------------------------------------------------------------------
struct BoneReference
{
	BoneReference(const short index, float weight, const string& name)
		: Index(index), Weight(weight), Name(name) {}
	short	Index;
	string	Name;
	float	Weight;

	template <class Archive>
	void serialize(Archive & ar)
	{
		ar(make_nvp("Index", Index), make_nvp("Weight", Weight));
	}
};

struct ControlPoint
{
	ControlPoint() {}
	ControlPoint(const CKFVec3& position) : Position(position) {}
	CKFVec3					Position;
	vector<BoneReference>	BoneReferences;
};

struct UVSet
{
	string			Name;
	vector<CKFVec2>	UVs;
	vector<us>		UVIndeces;
};

#ifdef USING_DIRECTX
struct VertexOutNoSkin
{
	CKFVec3	 Position;
	CKFVec3	 Normal;
	CKFVec2	 UV;
	CKFColor Color;

	template <class Archive>
	void serialize(Archive & ar)
	{
		ar(make_nvp("Position", Position), make_nvp("Normal", Normal), make_nvp("Color", Color), make_nvp("UV", UV));
	}
};

struct VertexOutSkin
{
	CKFVec3					Position;
	CKFVec3					Normal;
	CKFVec2					UV;
	CKFColor				Color;
	Short4					BoneIndexes;
	CKFVec4					BoneWeights;

	template <class Archive>
	void serialize(Archive & ar)
	{
		ar(make_nvp("Position", Position), make_nvp("Normal", Normal), make_nvp("Color", Color)
			, make_nvp("UV", UV), make_nvp("BoneIndexes", BoneIndexes), make_nvp("BoneWeights", BoneWeights));
	}
};

struct VertexDX
{
	VERTEX_3D				Vertex;
	vector<BoneReference>	BoneReferences;

	bool operator==(const VertexDX& vValue) const;
	operator VertexOutNoSkin () const
	{
		VertexOutNoSkin result;
		result.Position = Vertex.vPos;
		result.Normal = Vertex.vNormal;
		result.Color = CKFMath::sc_cWhite;
		result.UV = Vertex.vUV;
		return result;
	}
	operator VertexOutSkin () const
	{
		VertexOutSkin result;
		result.Position = Vertex.vPos;
		result.Normal = Vertex.vNormal;
		result.Color = CKFMath::sc_cWhite;
		result.UV = Vertex.vUV;
		for (int count = 0; count < BoneReferences.size(); ++count)
		{
			result.BoneIndexes.m_[count] = BoneReferences[count].Index;
			result.BoneWeights.m_[count] = BoneReferences[count].Weight;
		}
		return result;
	}
};

#endif

struct Mesh
{
	Mesh()
		: EnableCullFace(false)
		, EnableLight(false)
		, EnableFog(true)
		, IsSkin(false)
		, MyRenderPriority(RenderPriority::RP_Default)
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
	string					MaterialName;
	bool					EnableCullFace;
	bool					EnableLight;
	bool					EnableFog;
	bool					IsSkin;
	RenderPriority			MyRenderPriority;

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
{//�R���C�_�[
	CS::COL_TYPE	Type;
	CKFVec3			Position;
	CKFVec3			Rotation;
	CKFVec3			Scale;

	template <class Archive>
	void serialize(Archive & ar)
	{
		ar(make_nvp("Type", Type), make_nvp("Position", Position), make_nvp("Rotation", Rotation), make_nvp("Scale", Scale));
	}
};

//--------------------------------------------------------------------------------
//  �N���X��`
//--------------------------------------------------------------------------------
class CMyNode
{
	friend class CKFUtilityFBX;

public:
	CMyNode()
		: Translation(CKFVec3(0.0f))
		, RotationOffset(CKFVec3(0.0f))
		, Scale(CKFVec3(1.0f))
		, ColliderMaterialID(1)
	{}
	~CMyNode() {}

	list<CMyNode*>	Children;
	string			Name;
	vector<string>	AttributeNames;

	// Offset
	CKFVec3			Translation;
	CKFVec3			RotationOffset;
	CKFQuaternion	Rotation;
	CKFVec3			Scale;

	// Matrix
	CKFMtx44		World;

	vector<Mesh>	Meshes;

	//Collider
	list<ColliderInfo>	Colliders;
	us					ColliderMaterialID;		//Collider Mat

	void Release(void);
	void RecursiveUpdateMatrix(const CKFMtx44& parent);
	void RecursiveUpdateSkin(const vector<Cluster>& clusters);
	void RecursiveDraw(const unordered_map<string, Material>& mapMaterial, const bool& drawSkeleton, const bool& drawMesh, const bool& drawCollider);
	void RecursiveRecombineMeshes(void);
	void RecursiveReverseTexV(void);
	void RecalculateMeshesBy(const CKFMtx44& matrix);
	void RecursiveMatchClusterID(const vector<Cluster>& avatar);
	void RecursiveSave(JSONOutputArchive& archive, const string& fileName, const bool& haveAnimator);
	void RecursiveSave(BinaryOutputArchive& archive, const string& fileName, const bool& haveAnimator);

private:
	void analyzePoint(FbxMesh* pMesh);
	void analyzeNormal(FbxMesh* pMesh);
	void analyzeUV(FbxMesh* pMesh);
	void analyzeMaterial(FbxMesh* pMesh);
	void analyzeCluster(FbxMesh* pMesh);
	void saveMeshJson(const Mesh& mesh, const string& meshName);
	void saveSkinMeshJson(const Mesh& mesh, const string& meshName);
	void saveMeshBinary(const Mesh& mesh, const string& meshName);
	void saveSkinMeshBinary(const Mesh& mesh, const string& meshName);

#ifdef USING_DIRECTX
	static LPD3DXMESH s_pMeshSphere;
	static LPD3DXMESH s_pMeshCube;
#endif
};