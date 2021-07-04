/*****************************************************************//**
 * \file   Texture.h
 * \brief  �e�N�X�`��
 * 
 * \author USAMI KOSHI
 * \date   2021/06/21
 *********************************************************************/
#pragma once

#include <cstdint>
#include <string>

/// @brief �e�N�X�`��ID
using TextureID = std::uint32_t;
/// @brief ���݂��Ȃ��e�N�X�`��ID
constexpr TextureID NONE_TEXTURE_ID = std::numeric_limits<TextureID>::max();

/// @class Texture
/// @brief �e�N�X�`��
class Texture
{
public:
	explicit Texture(const TextureID& id, const std::string& name):
		m_id(id),
		m_name(name)
	{}

	std::string		m_name;
	TextureID		m_id;
};
