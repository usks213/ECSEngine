/*****************************************************************//**
 * \file   Shader.cpp
 * \brief  シェーダ
 * 
 * \author USAMI KOSHI
 * \date   2021/06/20
 *********************************************************************/

#include "Shader.h"

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
bool hasStaderStage(const std::uint32_t& shaderStageFlags, const EShaderStage& shaderStage) {
	return shaderStageFlags & static_cast<std::uint32_t>(ConvertShaderStage2Flags(shaderStage));
}