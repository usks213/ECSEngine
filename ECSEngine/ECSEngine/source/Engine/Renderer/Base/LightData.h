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
	Vector4 color;		 // 色
	Vector4 ambient;	 // 環境光
	Vector4 direction;	 // 方向
};

struct PointLightData
{
	Vector4 color;		 // 色
	Vector3 position;    // 位置
	float	range;       // 範囲
	Vector4 attenuation; // 減衰
};

struct SpotLightData
{
	Vector4 color;		 // 色
	Vector3 position;    // 位置
	float	range;       // 範囲
	Vector4 attenuation; // 減衰
	Vector3 direction;   // 方向
	float	spot;        // スポット
};