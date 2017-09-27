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
enum RENDER_PRIORITY
{//�����_�[�D��x
	RP_3D = 0,
	RP_3D_ALPHATEST,
	RP_3D_ZSORT,
	RP_MAX
};

enum RENDER_STATE
{//�����_�[�X�e�[�g
	RS_LIGHTOFF_CULLFACEON_MUL = 0,	//���C�g�I�t�A���ʕ`��A��Z����
	RS_LIGHTOFF_CULLFACEOFF_MUL,	//���C�g�I�t�A���ʕ`��A��Z����
	RS_LIGHTON_CULLFACEON_MUL,		//���C�g�I���A�Жʕ`��A��Z����
	RS_LIGHTON_CULLFACEOFF_MUL,		//���C�g�I���A�Жʕ`��A��Z����
	RS_MAX
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
	static void		AnalyzeFilePath(const string& strTexPath, string& strName, string& strType);
	static void		CorrectTexType(string& strTexType);
	static bool		CheckIsTexture(const string& strTexType);

private:
	CKFUtility() {}
	~CKFUtility() {}
};