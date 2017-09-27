//--------------------------------------------------------------------------------
//	便利関数
//　KF_Utility.h
//	Author : Xu Wenjie
//	Date   : 2017-08-18
//--------------------------------------------------------------------------------
#pragma once

//--------------------------------------------------------------------------------
//  構造体定義
//--------------------------------------------------------------------------------
enum RENDER_PRIORITY
{//レンダー優先度
	RP_3D = 0,
	RP_3D_ALPHATEST,
	RP_3D_ZSORT,
	RP_MAX
};

enum RENDER_STATE
{//レンダーステート
	RS_LIGHTOFF_CULLFACEON_MUL = 0,	//ライトオフ、両面描画、乗算合成
	RS_LIGHTOFF_CULLFACEOFF_MUL,	//ライトオフ、両面描画、乗算合成
	RS_LIGHTON_CULLFACEON_MUL,		//ライトオン、片面描画、乗算合成
	RS_LIGHTON_CULLFACEOFF_MUL,		//ライトオン、片面描画、乗算合成
	RS_MAX
};

//--------------------------------------------------------------------------------
//  クラス宣言
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