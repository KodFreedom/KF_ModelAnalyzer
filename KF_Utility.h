//--------------------------------------------------------------------------------
//	�֗��֐�
//�@KF_Utility.h
//	Author : Xu Wenjie
//	Date   : 2017-08-18
//--------------------------------------------------------------------------------
#pragma once

//--------------------------------------------------------------------------------
//  �\���̒�`
//--------------------------------------------------------------------------------
#if defined(USING_DIRECTX)
enum DrawType
{
	PointList = 1,
	LineList = 2,
	LineStrip = 3,
	TriangleList = 4,
	TriangleStrip = 5,
	TriangleFan = 6,
};
#else
enum DrawType
{
	PointList = 0,
	LineList = 1,
	LineStrip = 3,
	TriangleList = 4,
	TriangleStrip = 5,
	TriangleFan = 6,
};
#endif

enum RenderPriority
{
	RP_Default = 0,
	RP_AlphaTest,
	RP_ZSort,
	RP_Max
};

// �V�F�[�_�[�^�C�v
enum ShaderType
{
	kDefaultShader = 0, // LightOn_CCW_Multi_Solid_FogOn
	kNoLightNoFog, // LightOff_CCW_Multi_Solid_FogOff
	kCullNone, // LightOn_None_Multi_Solid_FogOn
	kShaderMax
};

enum MeshType // ���b�V���̃^�C�v
{
	k3dMesh = 0,
	k3dSkin
};

//--------------------------------------------------------------------------------
//  �N���X�錾
//--------------------------------------------------------------------------------
class CKFUtility
{
public:
	static int		GetStrToken(FILE* pFp, const string& strToken, string& strBuf);
	static int		GetStrToken(string& str, const string& strToken, string& strBuf);
	static int		GetStrCount(FILE* pFp, const string& strToken, const string& strComp);
	static int		GetStrCount(string& str, const string& strToken, const string& strComp);
	static string	GetFileName(const string& strFilePath);
	static string	ParameterNameToMethodName(const string& parameterName);
	static void		AnalyzeFilePath(const string& strTexPath, string& strName, string& strType);
	static void		CorrectTexType(string& strTexType);
	static bool		CheckIsTexture(const string& strTexType);
	static int		GetStringUntilString(FILE* filePointer, const string& compare, string& buffer);

private:
	CKFUtility() {}
	~CKFUtility() {}
};