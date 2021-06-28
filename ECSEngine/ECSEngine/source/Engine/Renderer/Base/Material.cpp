/*****************************************************************//**
 * \file   Material.cpp
 * \brief  �}�e���A��
 * 
 * \author USAMI KOSHI
 * \date   2021/06/20
 *********************************************************************/

#include "Material.h"


 /// @brief �R���X�g���N�^
Material::Material(const MaterialID& id, const std::string& name, const Shader& shader) :
	m_id(id),
	m_name(name),
	m_isTransparent(false),
	m_blendState(EBlendState::NONE),
	m_depthStencilState(EDepthStencilState::ENABLE_TEST_AND_ENABLE_WRITE),
	m_rasterizeState(ERasterizeState::CULL_BACK),
	m_shaderID(shader.m_id),
	m_pShader(const_cast<Shader*>(&shader))
{
	// �V�F�[�_����}�e���A���f�[�^�𐶐�
	for (EShaderStage stage = EShaderStage::VS; stage < EShaderStage::MAX; ++stage)
	{
		//--- CBuffer�m��
		auto stageIndex = static_cast<size_t>(stage);
		for (const auto& cbLayout : shader.m_cbufferLayouts[stageIndex])
		{
			auto& cbData = m_cbufferData[stageIndex][cbLayout.first];
			// ���O
			cbData.name = cbLayout.second.name;
			// �o�b�t�@�f�[�^�m��
			cbData.data = std::make_unique<std::byte[]>(cbLayout.second.size);
			// �o�b�t�@�T�C�Y
			cbData.size = cbLayout.second.size;
			// �X�V�t���O
			cbData.isUpdate = true;

			// �ϐ��f�[�^�i�[
			for (const auto& var : cbLayout.second.variables)
			{
				const auto& defaltVar = shader.m_cbufferDefaults.find(var.name);
				// �������q�f�[�^������ꍇ�͏���������
				if (defaltVar != shader.m_cbufferDefaults.end())
				{
					std::memcpy(cbData.data.get() + var.offset, 
						defaltVar->second.get(), var.size);	// �������c
				}
				else
				{
					std::memset(cbData.data.get() + var.offset, 0, var.size);
				}
			}
		}

		//--- �e�N�X�`��������
		for (const auto& tex : shader.m_textureBindDatas[stageIndex])
		{
			m_textureData[stageIndex][tex.second.slot] = NONE_TEXTURE_ID;
		}

		//--- �T���v��
		for (const auto& smp : shader.m_samplerBindDatas[stageIndex])
		{
			m_samplerData[stageIndex][smp.second.slot] = ESamplerState::NONE;
		}
	}
}

/// @brief �f�[�^�ݒ�
void Material::setData(const char* name, const void* data)
{
	// ����
	for (EShaderStage stage = EShaderStage::VS; stage < EShaderStage::MAX; ++stage)
	{
		auto stageIndex = static_cast<size_t>(stage);
		for (const auto& cbLayout : m_pShader->m_cbufferLayouts[stageIndex])
		{
			bool bBreak = false;
			// �ϐ��f�[�^
			for (const auto& var : cbLayout.second.variables)
			{
				// ��v����
				if (var.name == name)
				{
					auto& cbData = m_cbufferData[stageIndex][cbLayout.first];
					std::memcpy(cbData.data.get() + var.offset, data, var.size);
					cbData.isUpdate = true;
					bBreak = true;
					break;
				}
			}
			if (bBreak) break;
		}
	}
}

/// @brief �e�N�X�`���ݒ�
void Material::setTexture(const char* name, const TextureID textureID)
{
	// ����
	for (EShaderStage stage = EShaderStage::VS; stage < EShaderStage::MAX; ++stage)
	{
		auto stageIndex = static_cast<size_t>(stage);
		for (const auto& texBind : m_pShader->m_textureBindDatas[stageIndex])
		{
			if (texBind.second.name == name)
			{
				auto& texData = m_textureData[stageIndex][texBind.first];
				texData = textureID;
				return;
			}
		}
	}
}

/// @brief �e�N�X�`���擾
TextureID Material::getTexture(const char* name)
{
	// ����
	for (EShaderStage stage = EShaderStage::VS; stage < EShaderStage::MAX; ++stage)
	{
		auto stageIndex = static_cast<size_t>(stage);
		for (const auto& texBind : m_pShader->m_textureBindDatas[stageIndex])
		{
			if (texBind.second.name == name)
			{
				auto& texData = m_textureData[stageIndex][texBind.first];
				return texData;
			}
		}
	}
}

/// @brief �T���v���ݒ�
void Material::setSampler(const char* name, const ESamplerState sampler)
{
	// ����
	for (EShaderStage stage = EShaderStage::VS; stage < EShaderStage::MAX; ++stage)
	{
		auto stageIndex = static_cast<size_t>(stage);
		for (const auto& samBind : m_pShader->m_samplerBindDatas[stageIndex])
		{
			if (samBind.second.name == name)
			{
				auto& samData = m_samplerData[stageIndex][samBind.first];
				samData = sampler;
				return;
			}
		}
	}
}

/// @brief �T���v���擾
ESamplerState Material::getSampler(const char* name)
{
	// ����
	for (EShaderStage stage = EShaderStage::VS; stage < EShaderStage::MAX; ++stage)
	{
		auto stageIndex = static_cast<size_t>(stage);
		for (const auto& samBind : m_pShader->m_samplerBindDatas[stageIndex])
		{
			if (samBind.second.name == name)
			{
				auto& samData = m_samplerData[stageIndex][samBind.first];
				return samData;
			}
		}
	}
}
