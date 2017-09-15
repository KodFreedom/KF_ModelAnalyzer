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

struct Mesh
{
	Mesh() : nMaterialIndex(-1) {}

	vector<ControlPoint>	vecPoint;
	vector<CKFVec3>			vecNormal;
	vector<UVSET>			vecUVSet;
	vector<unsigned short>	vecPointIdx;
	vector<unsigned short>	vecNormalIdx;
	int						nMaterialIndex;
	vector<CKFMtx44>		vecMtx;
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
	{
		listChild.clear();
		strName.clear();
		vecAttributeName.clear();
		vecTex.clear();
		vecMesh.clear();
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

	void Release(void);
	void RecursiveDraw(const bool& bDrawNormal);

private:
	void		analyzePos(FbxMesh* pMesh);
	void		analyzeNormal(FbxMesh* pMesh);
	void		analyzeUV(FbxMesh* pMesh);
	void		analyzeTexture(FbxNode* pNode);
	void		analyzeMaterial(FbxMesh* pMesh);
	void		analyzeCluster(FbxMesh* pMesh);
	FbxAMatrix	getGeometry(FbxNode* pNode);
};

class CKFUtilityFBX
{
public:
	static CMyNode*	Load(const string& strFilePath);
	static void		ReCalculateVtx(CMyNode* pNode);

private:
	CKFUtilityFBX() {}
	~CKFUtilityFBX() {}

	static CMyNode*	recursiveNode(FbxManager* pManager, FbxNode* pNode);
	static string	getAttributeTypeName(FbxNodeAttribute::EType type);
};