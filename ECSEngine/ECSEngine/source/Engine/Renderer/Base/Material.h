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
#include <Engine/Utility/Mathf.h>


/// @brief �}�e���A��ID
using MaterialID = std::uint32_t;
/// @brief ���݂��Ȃ��}�e���A��ID
constexpr MaterialID NONE_MATERIAL_ID = std::numeric_limits<MaterialID>::max();


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

	/// @brief Texture�f�[�^
	struct TextureData
	{
		std::string name;
		TextureID	id;
	};

	struct SamplerData
	{
		std::string		name;
		ESamplerState	state;
	};

	/// @brief �S�X�e�[�W�A�X���b�g����CBuffer�f�[�^
	std::array<std::unordered_map<std::uint32_t, CBuffer>,
		static_cast<size_t>(EShaderStage::MAX)>	m_cbufferData;
	/// @brief �S�X�e�[�W�A�X���b�g�̃o�C���h����e�N�X�`��
	std::array<std::unordered_map<std::uint32_t, TextureData>,
		static_cast<size_t>(EShaderStage::MAX)>	m_textureData;
	/// @brief �S�X�e�[�W�A�X���b�g���̃T���v���X�e�[�g
	std::array<std::unordered_map<std::uint32_t, SamplerData>,
		static_cast<size_t>(EShaderStage::MAX)>	m_samplerData;

	/// @brief CBuffer�ϐ��f�[�^
	std::unordered_map<std::string, Shader::CBufferVariable> m_cbufferVariable;

public:
    /// @brief float�ݒ�
	void setFloat(const char* name, const float& data) { setData(name, &data); }

    /// @brief Vector2�ݒ�
    void setVector2(const char* name, const Vector2& data) { setData(name, &data); }

    /// @brief Vector3�ݒ�
    void setVector3(const char* name, const Vector3& data) { setData(name, &data); }

    /// @brief Vector4�ݒ�
    void setVector4(const char* name, const Vector4& data) { setData(name, &data); }

    /// @brief Matrix�ݒ�
    void setMatrix(const char* name, const Matrix& data) { setData(name, &data); }

    /// @brief Struct�ݒ�
    void setStruct(const char* name, const void* data) { setData(name, &data); }

    /// @brief �e�N�X�`���ݒ�
    void setTexture(const char* name, const TextureID textureID);

    /// @brief �e�N�X�`���擾
    TextureID getTexture(const char* name);

    /// @brief �T���v���ݒ�
    void setSampler(const char* name, const ESamplerState sampler);

    /// @brief �T���v���擾
    ESamplerState getSampler(const char* name);

	/// @brief �f�[�^�擾
	void* getData(const char* name);

protected:
	/// @brief �f�[�^�ݒ�
	void setData(const char* name, const void* data);

};