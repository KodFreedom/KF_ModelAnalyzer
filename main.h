//--------------------------------------------------------------------------------
//
//　main.h
//	Author : Xu Wenjie
//	Date   : 2017-04-26
//--------------------------------------------------------------------------------
#pragma once

#define NOMINMAX
//--------------------------------------------------------------------------------
//  インクルードファイル
//--------------------------------------------------------------------------------
#include <Windows.h>
#include <stdio.h>

//Include STL
#include <list>
#include <vector>
#include <unordered_map>
#include <string>
#include <algorithm>
#include <sstream>
#include <assert.h>
#include <fstream>
#include <iostream>
#include <map>
#include <unordered_map>
using namespace std;

//Cereal
#include <cereal/archives/JSON.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/string.hpp>
using namespace cereal;

//FBX
#include <fbxsdk.h>

//OpenMP
//#include <omp.h>

//KF関数
#include "KF_Math.h"
#include "KF_Utility.h"

//DirectX系
#ifdef USING_DIRECTX
#include <d3dx9.h>
#define DIRECTINPUT_VERSION (0x0800)
#include <dinput.h>
#include <XAudio2.h>//sound
#endif

//--------------------------------------------------------------------------------
//  ライブラリ読み込み
//--------------------------------------------------------------------------------
#ifdef USING_DIRECTX
#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"dinput8.lib")
#endif

#pragma comment(lib,"winmm.lib")	//システム時刻取得に必要
#pragma comment(lib,"libfbxsdk-mt.lib")

//--------------------------------------------------------------------------------
//  定数定義
//--------------------------------------------------------------------------------
typedef unsigned short		us;
typedef unsigned long		ul;
#define SAFE_RELEASE(p)		if(p){ p->Release(); p=nullptr; }	//safe release mode
#define SCREEN_WIDTH		(1280)								//ウインドウ幅
#define SCREEN_HEIGHT		(720)								//ウインドウ高さ
#define FRAME_PER_SECOND	(60)								
#define TIMER_INTERVAL		(1000.0 / FRAME_PER_SECOND)		//更新間隔
#define DELTA_TIME			((float)TIMER_INTERVAL * 0.001f)			//更新間隔(秒単位)
#define CLASS_NAME			"KF_Framework"						//クラスの名前
#define WINDOW_NAME			"KF_ModelAnalyzer"					//ウインドウの名前

#ifdef USING_DIRECTX
#define	FVF_VERTEX_2D	(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1)					//頂点フォーマット
#define FVF_VERTEX_3D	(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1 )	//3D頂点フォーマット
#endif											

//--------------------------------------------------------------------------------
//  前方宣言
//--------------------------------------------------------------------------------
class CManager;

//--------------------------------------------------------------------------------
//  構造体定義
//--------------------------------------------------------------------------------
typedef struct
{
	CKFVec3			vPos;	//xyz
	float			fRhw;	//rhw
	unsigned long	ulColor;//色
	CKFVec2			vUV;	//テクスチャ
}VERTEX_2D;//2D頂点情報構造体　頂点フォーマットと構造を合わせること

typedef struct//頂点情報構造体　上の頂点フォーマットと構造を合わせること
{
	CKFVec3			vPos;	//xyz
	CKFVec3			vNormal;//法線
	unsigned long	ulColor;//色
	CKFVec2			vUV;	//uv
}VERTEX_3D;//3D頂点情報構造体　頂点フォーマットと構造を合わせること

//--------------------------------------------------------------------------------
//  クラス宣言
//--------------------------------------------------------------------------------
class CMain
{
public:
	//--------------------------------------------------------------------------------
	//  関数定義
	//--------------------------------------------------------------------------------
	static int				Main(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR IpCmdLine, int nCmdShow);
	static bool				OpenModelFile(string& strFileName);
	static bool				OpenTextureFile(string& strTexName);

	//Get関数
	static CManager*		GetManager(void) { return m_pManager; }

private:
	//--------------------------------------------------------------------------------
	//  関数定義
	//--------------------------------------------------------------------------------
	CMain() {}
	~CMain() {}

	static LRESULT CALLBACK	wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static void				closeApp(HWND hWnd);

	//--------------------------------------------------------------------------------
	//  変数定義
	//--------------------------------------------------------------------------------
	static CManager*		m_pManager;
	static HWND				m_hWnd;
};
