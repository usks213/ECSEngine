/*****************************************************************//**
 * \file   DepthStencil.h
 * \brief  深度ステンシル
 * 
 * \author USAMI KOSHI
 * \date   2021/08/07
 *********************************************************************/
#pragma once

#include <cstdint>
#include <string>

 /// @brief 深度ステンシルID
using DepthStencilID = std::uint32_t;
/// @brief 存在しない深度ステンシルID
constexpr DepthStencilID NONE_DEPTH_STENCIL_ID = std::numeric_limits<DepthStencilID>::max();

/// @class DepthStencil
/// @brief 深度ステンシル
class DepthStencil
{
public:
	/// @brief コンストラクタ
	/// @param id 深度ステンシルID
	/// @param name 名前
	explicit DepthStencil(const DepthStencilID& id, const std::string& name) :
		m_id(id), m_name(name)
	{
	}

	/// @brief デストラクタ
	~DepthStencil() = default;

private:
	/// @brief 深度ステンシルID
	DepthStencilID	m_id;
	/// @brief 名前
	std::string		m_name;
};

