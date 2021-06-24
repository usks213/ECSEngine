/*****************************************************************//**
 * \file   WorldManager.cpp
 * \brief  ���[���h�}�l�[�W���[�̃x�[�X�N���X
 *
 * \author USAMI KOSHI
 * \date   2021/06/14
 *********************************************************************/

#include "WorldManager.h"
#include <Engine/Engine.h>

#include <Engine/Renderer/D3D11/D3D11RendererManager.h>
#include <Engine/Renderer/D3D11/D3D11Shader.h>
#include <Engine/Utility/HashUtil.h>
#include <string>


void WorldManager::initialize()
{
	auto* renderer = static_cast<D3D11RendererManager*>(m_pEngine->getRendererManager());
	auto* device = renderer->m_d3dDevice.Get();

	// �V�F�[�_�ǂݍ���
	ShaderDesc shaderDesc;
	shaderDesc.m_name = "Test";
	shaderDesc.m_stages = EShaderStageFlags::VS | EShaderStageFlags::PS;
	D3D11Shader shader(device, shaderDesc);

	// ���_���C�A�E�g���璸�_����
	for (auto var : shader.m_inputLayoutVariableList)
	{
		std::size_t type = hash::crc32string(var.semanticName.c_str());
		auto& format = var.formatType;

		if (type == hash::crc32string("POSITION"))
		{
			switch (format)
			{
			case D3D11Shader::InputLayoutVariable::FormatType::R32:
				break;
			default:
				break;
			}
		}
		else if (type == hash::crc32string("NORMAL"))
		{

		}
		else if (type == hash::crc32string("TANGENT"))
		{

		}
		else if (type == hash::crc32string("COLOR"))
		{

		}
		else if (type == hash::crc32string("TEXCOORD"))
		{

		}
	}
	D3D11_BUFFER_DESC vertex;

	//renderer->m_d3dDevice->CreateBuffer();
}

void WorldManager::finalize()
{

}

void WorldManager::fixedUpdate()
{

}

void WorldManager::update()
{

}

void WorldManager::render()
{

}


