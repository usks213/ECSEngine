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
	m_shaderID(shader.m_id)
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
			m_textureData[stageIndex][tex.second.slot] = 
				std::numeric_limits<std::uint32_t>::max();
		}

		//--- �T���v��
		for (const auto& smp : shader.m_samplerBindDatas[stageIndex])
		{
			m_samplerData[stageIndex][smp.second.slot] = ESamplerState::NONE;
		}
	}
}
