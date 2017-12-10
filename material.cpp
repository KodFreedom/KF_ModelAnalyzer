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
#include "material.h"

//--------------------------------------------------------------------------------
//  SaveAsJson
//--------------------------------------------------------------------------------
void Material::SaveAsJson(const unordered_map<string, Material>& mapMaterial)
{
	for (auto& pair : mapMaterial)
	{
		auto& filePath = "data/material/" + pair.first + ".json";
		ofstream file(filePath);
		if (!file.is_open()) return;
		JSONOutputArchive archive(file);
		archive(make_nvp("ColorTexture", pair.second.ColorTexture));
		archive(make_nvp("DiffuseTexture", pair.second.DiffuseTexture));
		archive(make_nvp("DiffuseTextureMask", pair.second.DiffuseTextureMask));
		archive(make_nvp("SpecularTexture", pair.second.SpecularTexture));
		archive(make_nvp("SpecularTextureMask", pair.second.SpecularTextureMask));
		archive(make_nvp("NormalTexture", pair.second.NormalTexture));
		archive(make_nvp("Ambient", pair.second.Ambient));
		archive(make_nvp("Diffuse", pair.second.Diffuse));
		archive(make_nvp("Specular", pair.second.Specular));
		archive(make_nvp("Emissive", pair.second.Emissive));
		archive(make_nvp("Power", pair.second.Power));
		file.close();
	}
}

//--------------------------------------------------------------------------------
//  SaveAsBinary
//--------------------------------------------------------------------------------
void Material::SaveAsBinary(const unordered_map<string, Material>& mapMaterial)
{
	for (auto& pair : mapMaterial)
	{
		auto& filePath = "data/material/" + pair.first + ".material";
		ofstream file(filePath, ios::binary);
		if (!file.is_open()) return;
		BinaryOutputArchive archive(file);

		int size = (int)pair.second.ColorTexture.size();
		archive.saveBinary(&size, sizeof(int));
		archive.saveBinary(&pair.second.ColorTexture[0], size);

		size = (int)pair.second.DiffuseTexture.size();
		archive.saveBinary(&size, sizeof(int));
		archive.saveBinary(&pair.second.DiffuseTexture[0], size);

		size = (int)pair.second.DiffuseTextureMask.size();
		archive.saveBinary(&size, sizeof(int));
		archive.saveBinary(&pair.second.DiffuseTextureMask[0], size);

		size = (int)pair.second.SpecularTexture.size();
		archive.saveBinary(&size, sizeof(int));
		archive.saveBinary(&pair.second.SpecularTexture[0], size);

		size = (int)pair.second.SpecularTextureMask.size();
		archive.saveBinary(&size, sizeof(int));
		archive.saveBinary(&pair.second.SpecularTextureMask[0], size);

		size = (int)pair.second.NormalTexture.size();
		archive.saveBinary(&size, sizeof(int));
		archive.saveBinary(&pair.second.NormalTexture[0], size);

		archive.saveBinary(&pair.second.Ambient, sizeof(CKFColor));
		archive.saveBinary(&pair.second.Diffuse, sizeof(CKFColor));
		archive.saveBinary(&pair.second.Specular, sizeof(CKFColor));
		archive.saveBinary(&pair.second.Emissive, sizeof(CKFColor));
		archive.saveBinary(&pair.second.Power, sizeof(float));
		file.close();
	}
}