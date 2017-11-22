//--------------------------------------------------------------------------------
//
//　Material.h
//	Author : Xu Wenjie
//	Date   : 2017-09-15
//--------------------------------------------------------------------------------
#pragma once

//--------------------------------------------------------------------------------
//  インクルードファイル
//--------------------------------------------------------------------------------
#include "main.h"

//--------------------------------------------------------------------------------
//  クラス定義
//--------------------------------------------------------------------------------
class Material
{
public:
	Material()
		: Diffuse(CKFColor(1.0f))
		, Ambient(CKFColor(0.2f, 0.2f, 0.2f, 1.0f))
		, Specular(CKFColor(0.0f, 0.0f, 0.0f, 1.0f))
		, Emissive(CKFColor(0.0f, 0.0f, 0.0f, 1.0f))
		, Power(1.0f) {}

	string	 DiffuseTextureName;
	string	 SpecularTextureName;
	string	 NormalTextureName;
	CKFColor Ambient;	// 環境光の反射率
	CKFColor Diffuse;	// 漫射光の反射率
	CKFColor Specular;	// 鏡面光の反射率
	CKFColor Emissive;	// 自発光
	float	 Power;		// ハイライトのシャープネス

	static void SaveAsJson(const unordered_map<string, Material>& mapMaterial);
	static void SaveAsBinary(const unordered_map<string, Material>& mapMaterial);
};