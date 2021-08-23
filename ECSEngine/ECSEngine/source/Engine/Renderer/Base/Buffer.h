/*****************************************************************//**
 * \file   Buffer.h
 * \brief  バッファ
 * 
 * \author USAMI KOSHI
 * \date   2021/08/22
 *********************************************************************/
#pragma once

#include "CommonState.h"
#include <numeric>
#include <string>

/// @brief バッファID
using BufferID = std::uint32_t;
/// @brief 存在しないバッファID
constexpr BufferID NONE_BUFFER_ID = std::numeric_limits<BufferID>::max();

/// @struct BufferData
/// @brief バッファ初期化データ
struct BufferData
{
	const void* pInitData = nullptr;
	std::size_t size	  = 0;
};

/// @struct BufferDesc
/// @brief バッファDesc
struct BufferDesc
{
	std::string		name;
	std::uint32_t	count			= 0;
	std::uint32_t	size			= 0;
	Usage			usage			= Usage::DEFAULT;
	BindFlags		bindFlags		= BindFlags::UNKNOWN;
	CPUAccessFlags	cpuAccessFlags	= CPUAccessFlags::NONE;
	MiscFlags		miscFlags		= MiscFlags::UNKNONE;
};

/// @class Buffer
/// @brief バッファ
class Buffer
{
public:
	/// @brief コンストラクタ
	/// @param id バッファID
	/// @param desc バッファDesc
	Buffer(const BufferID& id, const BufferDesc& desc) :
		m_id(id), m_desc(desc)
	{
	}
	/// @brief デストラクタ
	~Buffer() = default;

public:
	/// @brief バッファID
	BufferID	m_id;
	/// @brief バッファDesc
	BufferDesc	m_desc;
};
