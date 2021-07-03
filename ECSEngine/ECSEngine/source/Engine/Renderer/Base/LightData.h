/*****************************************************************//**
 * \file   LightData.h
 * \brief  ライトデータ
 * 
 * \author USAMI KOSHI
 * \date   2021/07/02
 *********************************************************************/
#pragma once

#include <Engine/Utility/Mathf.h>

 // ライトデータ

struct DirectionalLightData
{
	Vector4 ambient;     // 環境光
	Vector4 diffuse;     // 拡散光
	Vector4 specular;    // 反射光

	Vector4 direction;
};

struct PointLightData
{
	Vector4 ambient;     // 環境光
	Vector4 diffuse;     // 拡散光
	Vector4 specular;    // 反射光

	Vector3 position;    // 位置
	float	range;       // 範囲
	Vector4 attenuation; // 減衰
};

struct SpotLightData
{
	Vector4 ambient;     // 環境光
	Vector4 diffuse;     // 拡散光
	Vector4 specular;    // 反射光

	Vector3 position;    // 位置
	float	range;       // 範囲
	Vector4 attenuation; // 減衰
	Vector3 direction;   // 方向
	float	spot;        // スポット
};