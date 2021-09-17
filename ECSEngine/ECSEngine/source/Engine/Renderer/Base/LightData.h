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
	Vector4 color		= Vector4(1,1,1,1);				// 色+強さ
	Vector4 ambient		= Vector4(0.1f, 0.1f, 0.1f, 1); // 環境光
	Vector4 direction	= Vector4(1.0f, -1.0f, 1.0f, 1);// 方向
};

struct PointLightData
{
	Vector4 color		= Vector4(1, 1, 1, 1);	// 色+強さ
	Vector3 position;							// 位置
	float	range		= 1.0f;					// 範囲
};

struct SpotLightData
{
	Vector4 color		= Vector4(1, 1, 1, 1);	// 色+強さ
	Vector3 position;							// 位置
	float	range		= 2.0f;					// 範囲
	Vector3 direction	= Vector3(0, -1, 0);;	// 方向
	float	spot		= 0.5f;					// 円錐の大きさ(0.0f~1.0f)
};