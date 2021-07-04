/*****************************************************************//**
 * \file   Texture.h
 * \brief  テクスチャ
 * 
 * \author USAMI KOSHI
 * \date   2021/06/21
 *********************************************************************/
#pragma once

#include <cstdint>
#include <string>

/// @brief テクスチャID
using TextureID = std::uint32_t;
/// @brief 存在しないテクスチャID
constexpr TextureID NONE_TEXTURE_ID = std::numeric_limits<TextureID>::max();

/// @class Texture
/// @brief テクスチャ
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
