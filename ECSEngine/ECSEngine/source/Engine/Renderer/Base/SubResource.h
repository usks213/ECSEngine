/*****************************************************************//**
 * \file   SubResource.h
 * \brief  サブリソース
 * 
 * \author USAMI KOSHI
 * \date   2021/07/28
 *********************************************************************/
#pragma once

#include <cstdint>


/// @class SubResource
/// @brief サブリソース
class SubResource
{
public:
	/// @brief コンストラクタ
	SubResource() :
		pData(nullptr), ByteWidth(0),
		RowPitch(0), DepthPitch(0)
	{}

	/// @brief デストラクタ
	virtual ~SubResource() = default;

	/// @brief データ
	void* getData() { return pData; }
	/// @brief １データのサイズ
	std::uint32_t getByteSize() { return ByteWidth; }
	/// @brief データ配列の幅
	std::uint32_t getWidth() { return RowPitch / getByteSize(); }
	/// @brief データ配列の高さ
	std::uint32_t getHeight() { return DepthPitch / getWidth(); }

public:
	void* pData;				// データ
	std::uint32_t ByteWidth;	// 1データのサイズ(バイト)
	std::uint32_t RowPitch;		// データの幅(バイト)
	std::uint32_t DepthPitch;	// データの高さ(バイト)
};
