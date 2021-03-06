//--------------------------------------------------------------------------------
//	便利関数
//　KF_Utility.h
//	Author : Xu Wenjie
//	Date   : 2017-08-18
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//  インクルードファイル
//--------------------------------------------------------------------------------
#include "main.h"
#include "KF_Utility.h"

//--------------------------------------------------------------------------------
//  クラス
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//	関数名：GetStrToken
//  関数説明：アクション（移動、跳ぶ、攻撃）
//	引数：	vDirection：移動方向
//			bJump：跳ぶフラグ
//	戻り値：なし
//--------------------------------------------------------------------------------
int CKFUtility::GetStrToken(FILE* pFp, const string& strToken, string& strBuf)
{
	char c;
	strBuf.clear();
	while ((c = (char)fgetc(pFp)) != EOF)
	{
		for (int nCnt = 0; nCnt < (int)strToken.length(); nCnt++)
		{
			if (c == strToken.at(nCnt))
			{
				return strBuf.length();
			}
		}
		strBuf += c;
	}
	return -1;
}

//--------------------------------------------------------------------------------
//	関数名：GetStrToken
//  関数説明：アクション（移動、跳ぶ、攻撃）
//	引数：	vDirection：移動方向
//			bJump：跳ぶフラグ
//	戻り値：なし
//--------------------------------------------------------------------------------
int CKFUtility::GetStrToken(string& str, const string& strToken, string& strBuf)
{
	strBuf.clear();
	for (auto itr = str.begin(); itr != str.end();)
	{
		auto c = *itr;
		itr = str.erase(itr);
		for (int nCnt = 0; nCnt < (int)strToken.length(); nCnt++)
		{
			if (c == strToken.at(nCnt))
			{
				return strBuf.length();
			}
		}
		strBuf += c;
	}

	return strBuf.length();
}

//--------------------------------------------------------------------------------
//	関数名：GetStrCount
//  関数説明：アクション（移動、跳ぶ、攻撃）
//	引数：	vDirection：移動方向
//			bJump：跳ぶフラグ
//	戻り値：なし
//--------------------------------------------------------------------------------
int CKFUtility::GetStrCount(FILE* pFp, const string& strToken, const string& strComp)
{
	int nCnt = 0;
	string strBuf;
	while (GetStrToken(pFp, strToken, strBuf) >= 0)
	{
		if (strBuf.compare(strComp) == 0)
		{
			nCnt++;
		}
	}

	//Fileの頭に戻る
	fseek(pFp, 0, SEEK_SET);

	return nCnt;
}

//--------------------------------------------------------------------------------
//	関数名：GetStrCount
//  関数説明：アクション（移動、跳ぶ、攻撃）
//	引数：	vDirection：移動方向
//			bJump：跳ぶフラグ
//	戻り値：なし
//--------------------------------------------------------------------------------
int CKFUtility::GetStrCount(string& str, const string& strToken, const string& strComp)
{
	int nCnt = 0;
	string strBuf;
	while (GetStrToken(str, strToken, strBuf) >= 0)
	{
		if (strBuf.compare(strComp) == 0)
		{
			nCnt++;
		}
	}
	return nCnt;
}

//--------------------------------------------------------------------------------
//	関数名：GetFileName
//  関数説明：アクション（移動、跳ぶ、攻撃）
//	引数：	vDirection：移動方向
//			bJump：跳ぶフラグ
//	戻り値：なし
//--------------------------------------------------------------------------------
string CKFUtility::GetFileName(const string& strFilePath)
{
	auto strCpy = strFilePath;

	//逆転
	reverse(strCpy.begin(), strCpy.end());

	//ファイル型の取得
	string strType;
	GetStrToken(strCpy, ".", strType);

	//ファイル名の取得
	string strName;
	GetStrToken(strCpy, "\\/", strName);
	reverse(strName.begin(), strName.end());
	return strName;
}

//--------------------------------------------------------------------------------
//	関数名：GetStrCount
//  関数説明：アクション（移動、跳ぶ、攻撃）
//	引数：	vDirection：移動方向
//			bJump：跳ぶフラグ
//	戻り値：なし
//--------------------------------------------------------------------------------
string CKFUtility::ParameterNameToMethodName(const string& parameterName)
{
	string copy = parameterName;
	string buffer;
	string result;
	while (GetStrToken(copy, "_\0", buffer) > 0)
	{
		if ((buffer[0] >= 'a') && (buffer[0] <= 'z'))
		{
			buffer[0] = buffer[0] - 32;
		}
		result += buffer;
	}
	return result;
}

//--------------------------------------------------------------------------------
//	関数名：GetStrCount
//  関数説明：アクション（移動、跳ぶ、攻撃）
//	引数：	vDirection：移動方向
//			bJump：跳ぶフラグ
//	戻り値：なし
//--------------------------------------------------------------------------------
void CKFUtility::AnalyzeFilePath(const string& strTexPath, string& strName, string& strType)
{
	auto strCpy = strTexPath;

	//逆転
	reverse(strCpy.begin(), strCpy.end());

	//ファイル型の取得
	if (GetStrToken(strCpy, ".", strType) > 0)
	{
		reverse(strType.begin(), strType.end());
	}

	//ファイル名の取得
	GetStrToken(strCpy, "\\/", strName);
	reverse(strName.begin(), strName.end());
}

//--------------------------------------------------------------------------------
//	関数名：CheckIsTexture
//  関数説明：アクション（移動、跳ぶ、攻撃）
//	引数：	vDirection：移動方向
//			bJump：跳ぶフラグ
//	戻り値：なし
//--------------------------------------------------------------------------------
bool CKFUtility::CheckIsTexture(const string& strTexType)
{
	if (strTexType._Equal("png")
		|| strTexType._Equal("PNG")
		|| strTexType._Equal("jpg")
		|| strTexType._Equal("JPG")
		|| strTexType._Equal("tga")
		|| strTexType._Equal("TGA")
		|| strTexType._Equal("tif")
		|| strTexType._Equal("TIF"))

	{
		return true;
	}

	return false;
}

//--------------------------------------------------------------------------------
//	関数名：CorrectTexType
//  関数説明：アクション（移動、跳ぶ、攻撃）
//	引数：	vDirection：移動方向
//			bJump：跳ぶフラグ
//	戻り値：なし
//--------------------------------------------------------------------------------
void CKFUtility::CorrectTexType(string& strTexType)
{
	if (strTexType._Equal("psd")
		|| strTexType._Equal("PSD"))
	{
		strTexType = "png";
	}
}

//--------------------------------------------------------------------------------
//	関数名：GetStringUntilString
//  関数説明：文字列まで文字列を取る
//	引数：	file：ファイルの文字列
//			compare：比較する文字
//			buffer：文字列保存用ところ
//	戻り値：ファイルが終わったら-1を返す、そうじゃないなら文字数を返す
//--------------------------------------------------------------------------------
int CKFUtility::GetStringUntilString(FILE* filePointer, const string& compare, string& buffer)
{
	static string nullBuffer;
	if (nullBuffer.empty()) nullBuffer.resize(256);
	do
	{
		buffer = nullBuffer;
		fgets(&buffer[0], (int)buffer.capacity(), filePointer);
		if (buffer.find(compare) != string::npos)
		{
			return buffer.length();
		}
	} while (buffer.find("END_SCRIPT") == string::npos);
	return -1;
}