/*****************************************************************//**
 * \file   Buffer.h
 * \brief  �o�b�t�@
 * 
 * \author USAMI KOSHI
 * \date   2021/08/22
 *********************************************************************/
#pragma once

#include "CommonState.h"
#include <numeric>
#include <string>

/// @brief �o�b�t�@ID
using BufferID = std::uint32_t;
/// @brief ���݂��Ȃ��o�b�t�@ID
constexpr BufferID NONE_BUFFER_ID = std::numeric_limits<BufferID>::max();

/// @struct BufferData
/// @brief �o�b�t�@�������f�[�^
struct BufferData
{
	const void* pInitData = nullptr;
	std::size_t size	  = 0;
};

/// @struct BufferDesc
/// @brief �o�b�t�@Desc
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
/// @brief �o�b�t�@
class Buffer
{
public:
	/// @brief �R���X�g���N�^
	/// @param id �o�b�t�@ID
	/// @param desc �o�b�t�@Desc
	Buffer(const BufferID& id, const BufferDesc& desc) :
		m_id(id), m_desc(desc)
	{
	}
	/// @brief �f�X�g���N�^
	~Buffer() = default;

public:
	/// @brief �o�b�t�@ID
	BufferID	m_id;
	/// @brief �o�b�t�@Desc
	BufferDesc	m_desc;
};
