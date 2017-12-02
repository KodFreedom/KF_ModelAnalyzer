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
		archive(make_nvp("DiffuseTextureName", pair.second.DiffuseTextureName));
		archive(make_nvp("SpecularTextureName", pair.second.SpecularTextureName));
		archive(make_nvp("NormalTextureName", pair.second.NormalTextureName));
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
		int size = (int)pair.second.DiffuseTextureName.size();
		archive.saveBinary(&size, sizeof(int));
		archive.saveBinary(&pair.second.DiffuseTextureName[0], size);
		size = (int)pair.second.SpecularTextureName.size();
		archive.saveBinary(&size, sizeof(int));
		archive.saveBinary(&pair.second.SpecularTextureName[0], size);
		size = (int)pair.second.NormalTextureName.size();
		archive.saveBinary(&size, sizeof(int));
		archive.saveBinary(&pair.second.NormalTextureName[0], size);
		archive.saveBinary(&pair.second.Ambient, sizeof(CKFColor));
		archive.saveBinary(&pair.second.Diffuse, sizeof(CKFColor));
		archive.saveBinary(&pair.second.Specular, sizeof(CKFColor));
		archive.saveBinary(&pair.second.Emissive, sizeof(CKFColor));
		archive.saveBinary(&pair.second.Power, sizeof(float));
		file.close();
	}
}