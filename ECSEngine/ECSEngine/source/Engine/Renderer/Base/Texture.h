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

struct Texture
{
	std::string		m_name;
	TextureID		m_id;
};
