/*****************************************************************//**
 * \file   ShaderBase.h
 * \brief  �V�F�[�_�[�̃x�[�X�N���X
 * 
 * \author USAMI KOSHI
 * \date   2021/06/18
 *********************************************************************/
#pragma once

#include <cstdint>
#include <limits>
#include <string>


/// @brief �V�F�[�_�[ID
using ShaderID = std::uint32_t;


/// @enum EShaderStage
/// @brief �V�F�[�_�[�X�e�[�W
enum class EShaderStage : std::uint32_t
{
	VS,		// ���_�V�F�[�_�[
	GS,		// �W�I���g���V�F�[�_�[
	DS,		// �h���C���V�F�[�_�[
	HS,		// �n���V�F�[�_�[
	PS,		// �s�N�Z���V�F�[�_�[
	CS,		// �R���s���[�g�V�F�[�_�[
	MAX,	// �V�F�[�_�[�X�e�[�W��
};

/// @enum EShaderStageFlags
/// @brief �V�F�[�_�[�X�e�[�W�t���O
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
constexpr bool hasStaderStage(const std::uint32_t& shaderStageFlags, const EShaderStage& shaderStage) {
	return shaderStageFlags & static_cast<std::uint32_t>(ConvertShaderStage2Flags(shaderStage));
}


/// @brief �V�F�[�_�[�����p�\����
struct ShaderDesc
{
	std::uint32_t	m_stages;	// �V�F�[�_�[�X�e�[�W
	std::string		m_name;		// �V�F�[�_�[��
	// �V�F�[�_�}�N��
};

struct ShaderBase
{
	/// @brief �R���X�g���N�^
	ShaderBase(ShaderDesc shaderDesc) :
		m_id(std::numeric_limits<ShaderID>::max()),
		m_desc(shaderDesc)
	{
	}

	/// @brief �f�X�g���N�^
	virtual ~ShaderBase() = default;


	ShaderID	m_id;
	ShaderDesc  m_desc;
	// �n�b�V���l
};
