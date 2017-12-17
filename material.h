//--------------------------------------------------------------------------------
//
//�@Material.h
//	Author : Xu Wenjie
//	Date   : 2017-09-15
//--------------------------------------------------------------------------------
#pragma once

//--------------------------------------------------------------------------------
//  �C���N���[�h�t�@�C��
//--------------------------------------------------------------------------------
#include "main.h"

//--------------------------------------------------------------------------------
//  �N���X��`
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

	string   ColorTexture;
	string	 DiffuseTexture;
	string	 DiffuseTextureMask;
	string	 SpecularTexture;
	string	 SpecularTextureMask;
	string	 NormalTexture;
	string   DetailTexture;
	string   DetailMask;
	string   TintByBaseMask;
	string   RimMask;
	string   Translucency;
	string   MetalnessMask;
	string   SelfIllumMask;
	string   FresnelWarpColor;
	string   FresnelWarpRim;
	string   FresnelWarpSpecular;
	CKFColor Ambient;	// �����̔��˗�
	CKFColor Diffuse;	// ���ˌ��̔��˗�
	CKFColor Specular;	// ���ʌ��̔��˗�
	CKFColor Emissive;	// ������
	float	 Power;		// �n�C���C�g�̃V���[�v�l�X

	static void SaveAsJson(const unordered_map<string, Material>& mapMaterial);
	static void SaveAsBinary(const unordered_map<string, Material>& mapMaterial);
};