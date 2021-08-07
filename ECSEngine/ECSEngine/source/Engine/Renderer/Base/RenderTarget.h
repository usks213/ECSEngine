/*****************************************************************//**
 * \file   RenderTarget.h
 * \brief  レンダーターゲット
 * 
 * \author USAMI KOSHI
 * \date   2021/08/07
 *********************************************************************/
#pragma once

#include <cstdint>
#include <string>

/// @brief レンダーターゲットID
using RenderTargetID = std::uint32_t;
/// @brief 存在しないレンダーターゲットID
constexpr RenderTargetID NONE_RENDER_TARGET_ID = std::numeric_limits<RenderTargetID>::max();


/// @class RenderTarget
/// @brief レンダーターゲット
class RenderTarget
{
public:
	/// @brief コンストラクタ
	/// @param id レンダーターゲットID
	/// @param name 名前
	explicit RenderTarget(const RenderTargetID& id, const std::string& name) :
		m_id(id), m_name(name)
	{
	}
	/// @brief デストラクタ
	~RenderTarget() = default;

	/// @brief レンダーターゲットテクスチャの取得
	/// @return テクスチャID
	//virtual const TextureID getRTTexture() const = 0;

public:
	/// @brief レンダーターゲットID
	RenderTargetID	m_id;
	/// @brief 名前
	std::string		m_name;
};
