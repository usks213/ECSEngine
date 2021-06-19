/*****************************************************************//**
 * \file   D3DUtility.h
 * \brief  DirectX11汎用
 * 
 * \author USAMI KOSHI
 * \date   2021/06/17
 *********************************************************************/
#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <array>
#include <algorithm>
#include <memory>

#include <comdef.h>
#include <Windows.h>
#include <DirectXMath.h>
#include <DirectXColors.h>

#include <wrl/client.h>
using Microsoft::WRL::ComPtr;



#define CHECK_FAILED(hr)													\
	if (FAILED(hr)) {														\
		static char szResult[256];                                          \
		_com_error  err(hr);                                                \
		LPCTSTR     errMsg = err.ErrorMessage();							\
        sprintf_s(szResult, sizeof(szResult), "[ERROR] HRESULT: %s",errMsg);\
		MessageBox(nullptr, szResult, "Err", MB_ICONSTOP);					\
		throw std::exception(szResult);     								\
	}

/// @enum ERasterizeState
/// @brief ラスタライザステート
enum class ERasterizeState : std::uint8_t
{
	UNKNOWN = 0,				// 不明
	CULL_NONE,				// カリングなし
	CULL_FRONT,				// 表面カリング
	CULL_BACK,				// 裏面カリング
	CULL_NONE_WIREFRAME,	// ワイヤーフレーム カリングなし
	CULL_FRONT_WIREFRAME,	// ワイヤーフレーム 表面カリング
	CULL_BACK_WIREFRAME,	// ワイヤーフレーム 裏面カリング
	SHADOW,					// シャドウ用
	MAX,					// ラスタライザステートの数
};

/// @enum ESamplerState
/// @brief サンプラステート
enum class ESamplerState : std::uint8_t
{
	NONE = 0,			// サンプラなし
	LINEAR_CLAMP,		// リニアクランプ
	POINT_CLAMP,		// ポイントクランプ
	ANISOTROPIC_CLAMP,	// 異方性クランプ
	LINEAR_WRAP,			// リニアラップ
	POINT_WRAP,			// ポイントラップ
	ANISOTROPIC_WRAP,	// 異方性ラップ
	SHADOW,				// シャドウ用
	MAX,				// サンプラステートの数
};

/// @enum EBlendState
/// @brief ブレンドステート
enum class EBlendState : std::uint8_t
{
	UNKNOWN = 0,	// 不明
	NONE,		// ブレンドなし
	ALPHA,		// 半透明合成
	ADD,		// 加算合成
	SUB,		// 減算合成
	MUL,		// 乗算合成
	INV,		// 反転合成
	MAX,		// ブレンドステートの数
};

/// @enum EDepthStencilState
/// @brief 深度ステンシルステート
enum class EDepthStencilState : std::uint8_t
{
	UNKNOWN = 0,
	ENABLE_TEST_AND_ENABLE_WRITE,	// 深度ステンシルテスト有効 & 深度ステンシル更新有効
	ENABLE_TEST_AND_DISABLE_WRITE,	// 深度ステンシルテスト有効 & 深度ステンシル更新無効
	DISABLE_TEST_AND_ENABLE_WRITE,	// 深度ステンシルテスト無効 & 深度ステンシル更新有効
	DISABLE_TEST_AND_DISABLE_WRITE,	// 深度ステンシルテスト無効 & 深度ステンシル更新無効
	MAX,							// 深度ステンシルステートの数
};

/// @enum EPrimitiveTopology
/// @brief プリミティブトポロジ
enum class EPrimitiveTopology : std::uint8_t
{
	UNKNOWN = 0,						// プリミティブトポロジ不明
	TRIANGLE_LIST,						// トライアングルリスト
	TRIANGLE_STRIP,						// トライアングルストリップ
	POINT_LIST,							// ポイントリスト
	LINE_LIST,							// ラインリスト
	LINE_STRIP,							// ラインストリップ
	TRIANGLE_CONTROL_POINT_PATCHLIST,	// トライアングルコントロールポイントパッチリスト
	MAX,								// プリミティブトポロジの数
};

