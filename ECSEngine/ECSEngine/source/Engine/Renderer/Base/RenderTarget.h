/*****************************************************************//**
 * \file   RenderTarget.h
 * \brief  �����_�[�^�[�Q�b�g
 * 
 * \author USAMI KOSHI
 * \date   2021/08/07
 *********************************************************************/
#pragma once

#include <cstdint>
#include <string>

/// @brief �����_�[�^�[�Q�b�gID
using RenderTargetID = std::uint32_t;
/// @brief ���݂��Ȃ������_�[�^�[�Q�b�gID
constexpr RenderTargetID NONE_RENDER_TARGET_ID = std::numeric_limits<RenderTargetID>::max();


/// @class RenderTarget
/// @brief �����_�[�^�[�Q�b�g
class RenderTarget
{
public:
	/// @brief �R���X�g���N�^
	/// @param id �����_�[�^�[�Q�b�gID
	/// @param name ���O
	explicit RenderTarget(const RenderTargetID& id, const std::string& name) :
		m_id(id), m_name(name)
	{
	}
	/// @brief �f�X�g���N�^
	~RenderTarget() = default;

	/// @brief �����_�[�^�[�Q�b�g�e�N�X�`���̎擾
	/// @return �e�N�X�`��ID
	//virtual const TextureID getRTTexture() const = 0;

public:
	/// @brief �����_�[�^�[�Q�b�gID
	RenderTargetID	m_id;
	/// @brief ���O
	std::string		m_name;
};
