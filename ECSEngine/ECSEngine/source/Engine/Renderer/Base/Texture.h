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

struct Texture
{
	std::string		m_name;
	TextureID		m_id;
};
