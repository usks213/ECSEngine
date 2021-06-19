/*****************************************************************//**
 * \file   ShaderBase.h
 * \brief  シェーダーのベースクラス
 * 
 * \author USAMI KOSHI
 * \date   2021/06/18
 *********************************************************************/
#pragma once

#include <cstdint>
#include <limits>
#include <string>


/// @brief シェーダーID
using ShaderID = std::uint32_t;


/// @enum EShaderStage
/// @brief シェーダーステージ
enum class EShaderStage : std::uint32_t
{
	VS,		// 頂点シェーダー
	GS,		// ジオメトリシェーダー
	DS,		// ドメインシェーダー
	HS,		// ハルシェーダー
	PS,		// ピクセルシェーダー
	CS,		// コンピュートシェーダー
	MAX,	// シェーダーステージ数
};

/// @enum EShaderStageFlags
/// @brief シェーダーステージフラグ
enum class EShaderStageFlags : std::uint32_t
{
	NONE = 0,
	VS = 1 << 1,
	GS = 1 << 2,
	DS = 1 << 3,
	HS = 1 << 4,
	PS = 1 << 5,
	CS = 1 << 6,
	MAX = std::numeric_limits<std::uint32_t>::max(),
	ALL = MAX,
};

/// @brief ループ用インクリメント
EShaderStage operator++(EShaderStage& value) {
	int result = static_cast<int>(value) + 1;
	if (result > static_cast<int>(EShaderStage::MAX)) {
		value = EShaderStage::VS;
		return EShaderStage::VS;
	}
	else {
		value = static_cast<EShaderStage>(result);
		return value;
	}
}
/// @brief ループ用インクリメント
EShaderStage operator++(EShaderStage& value, int) {
	int result = static_cast<int>(value) + 1;
	if (result > static_cast<int>(EShaderStage::MAX)) {
		value = EShaderStage::VS;
		return EShaderStage::VS;
	}
	else {
		value = static_cast<EShaderStage>(result);
		return static_cast<EShaderStage>(result);
	}
}
/// @brief シェーダーステージフラグ用論理和
std::uint32_t operator|(const EShaderStageFlags& l, const EShaderStageFlags& r) {
	return static_cast<std::uint32_t>(l) | static_cast<std::uint32_t>(r);
}
/// @brief シェーダーステージフラグ用論理和
std::uint32_t operator|(const std::uint32_t& l, const EShaderStageFlags& r) {
	return l | static_cast<std::uint32_t>(r);
}

/// @brief 
/// @param shaderStage 
/// @return 
constexpr EShaderStageFlags ConvertShaderStage2Flags(const EShaderStage& shaderStage) {
	switch (shaderStage) {
	case EShaderStage::VS: return EShaderStageFlags::VS;
	case EShaderStage::GS: return EShaderStageFlags::GS;
	case EShaderStage::DS: return EShaderStageFlags::DS;
	case EShaderStage::HS: return EShaderStageFlags::HS;
	case EShaderStage::PS: return EShaderStageFlags::PS;
	case EShaderStage::CS: return EShaderStageFlags::CS;
	default:			   return EShaderStageFlags::NONE;
	}
}
/// @brief シェーダーステージがフラグに含まれているか
/// @details FlagsとANDを取る
/// @param shaderStageFlags シェーダーステージフラグ
/// @param shaderStage シェーダーステージ
/// @return 含まれているならTRUE
constexpr bool hasStaderStage(const std::uint32_t& shaderStageFlags, const EShaderStage& shaderStage) {
	return shaderStageFlags & static_cast<std::uint32_t>(ConvertShaderStage2Flags(shaderStage));
}


/// @brief シェーダー生成用構造体
struct ShaderDesc
{
	std::uint32_t	m_stages;	// シェーダーステージ
	std::string		m_name;		// シェーダー名
	// シェーダマクロ
};

struct ShaderBase
{
	/// @brief コンストラクタ
	ShaderBase(ShaderDesc shaderDesc) :
		m_id(std::numeric_limits<ShaderID>::max()),
		m_desc(shaderDesc)
	{
	}

	/// @brief デストラクタ
	virtual ~ShaderBase() = default;


	ShaderID	m_id;
	ShaderDesc  m_desc;
	// ハッシュ値
};
