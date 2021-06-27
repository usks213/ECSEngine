/*****************************************************************//**
 * \file   Material.h
 * \brief  �}�e���A��
 * 
 * \author USAMI KOSHI
 * \date   2021/06/20
 *********************************************************************/
#pragma once

#include "CommonState.h"
#include "Shader.h"
#include "Texture.h"


/// @brief �}�e���A��ID
using MaterialID = std::uint32_t;

/// @class Material
/// @brief �}�e���A��
class Material
{
public:
	/// @brief �R���X�g���N�^
	/// @param �V�F�[�_���
	explicit Material(const MaterialID& id, const std::string& name, const Shader& shader);

	/// @brief �f�X�g���N�^
	virtual ~Material() = default;


public:
	/// @brief �}�e���A��ID
	MaterialID			m_id;
	/// @brief �}�e���A����
	std::string			m_name;
	/// @brief �������t���O
	bool				m_isTransparent;
	/// @brief �u�����h�X�e�[�g(�������̂�)
	EBlendState			m_blendState;
	/// @brief �[�x�X�e���V���X�e�[�g
	EDepthStencilState	m_depthStencilState;
	/// @brief ���X�^���C�U�X�e�[�g
	ERasterizeState		m_rasterizeState;
	/// @brief �V�F�[�_ID
	ShaderID			m_shaderID;

	/// @brief CBuffer�f�[�^
	struct CBuffer
	{
		std::string						name;
		std::unique_ptr<std::byte[]>	data;
		std::size_t						size;
		bool							isUpdate;	//true�Ȃ�X�V����
	};

	/// @brief �S�X�e�[�W�A�X���b�g����CBuffer�f�[�^
	std::array<std::unordered_map<std::uint32_t, CBuffer>,
		static_cast<size_t>(EShaderStage::MAX)>	m_cbufferData;
	/// @brief �S�X�e�[�W�A�X���b�g�̃o�C���h����e�N�X�`��ID
	std::array<std::unordered_map<std::uint32_t, TextureID>,
		static_cast<size_t>(EShaderStage::MAX)>	m_textureData;
	/// @brief �S�X�e�[�W�A�X���b�g���̃T���v���X�e�[�g
	std::array<std::unordered_map<std::uint32_t, ESamplerState>,
		static_cast<size_t>(EShaderStage::MAX)>	m_samplerData;
};