/*****************************************************************//**
 * \file   DepthStencil.h
 * \brief  �[�x�X�e���V��
 * 
 * \author USAMI KOSHI
 * \date   2021/08/07
 *********************************************************************/
#pragma once

#include <cstdint>
#include <string>

 /// @brief �[�x�X�e���V��ID
using DepthStencilID = std::uint32_t;
/// @brief ���݂��Ȃ��[�x�X�e���V��ID
constexpr DepthStencilID NONE_DEPTH_STENCIL_ID = std::numeric_limits<DepthStencilID>::max();

/// @class DepthStencil
/// @brief �[�x�X�e���V��
class DepthStencil
{
public:
	/// @brief �R���X�g���N�^
	/// @param id �[�x�X�e���V��ID
	/// @param name ���O
	explicit DepthStencil(const DepthStencilID& id, const std::string& name) :
		m_id(id), m_name(name)
	{
	}

	/// @brief �f�X�g���N�^
	~DepthStencil() = default;

private:
	/// @brief �[�x�X�e���V��ID
	DepthStencilID	m_id;
	/// @brief ���O
	std::string		m_name;
};

