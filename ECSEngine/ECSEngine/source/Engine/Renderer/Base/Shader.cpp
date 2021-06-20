/*****************************************************************//**
 * \file   Shader.cpp
 * \brief  �V�F�[�_
 * 
 * \author USAMI KOSHI
 * \date   2021/06/20
 *********************************************************************/

#include "Shader.h"

 /// @brief ���[�v�p�C���N�������g
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
/// @brief ���[�v�p�C���N�������g
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
/// @brief �V�F�[�_�[�X�e�[�W�t���O�p�_���a
std::uint32_t operator|(const EShaderStageFlags& l, const EShaderStageFlags& r) {
	return static_cast<std::uint32_t>(l) | static_cast<std::uint32_t>(r);
}
/// @brief �V�F�[�_�[�X�e�[�W�t���O�p�_���a
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
/// @brief �V�F�[�_�[�X�e�[�W���t���O�Ɋ܂܂�Ă��邩
/// @details Flags��AND�����
/// @param shaderStageFlags �V�F�[�_�[�X�e�[�W�t���O
/// @param shaderStage �V�F�[�_�[�X�e�[�W
/// @return �܂܂�Ă���Ȃ�TRUE
bool hasStaderStage(const std::uint32_t& shaderStageFlags, const EShaderStage& shaderStage) {
	return shaderStageFlags & static_cast<std::uint32_t>(ConvertShaderStage2Flags(shaderStage));
}