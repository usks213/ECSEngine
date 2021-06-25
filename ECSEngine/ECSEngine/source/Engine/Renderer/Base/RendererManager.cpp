/*****************************************************************//**
 * \file   RendererManager.cpp
 * \brief  �����_���[�}�l�[�W���[�̃x�[�X�N���X
 *
 * \author USAMI KOSHI
 * \date   2021/06/14
 *********************************************************************/

#include "RendererManager.h"



Shader* RendererManager::getShader(ShaderID id)
{
	const auto& itr = m_shaderPool.find(id);
	if (m_shaderPool.end() == itr) return nullptr;

	return itr->second.get();
}

Material* RendererManager::getMaterial(MaterialID id)
{
	const auto& itr = m_materialPool.find(id);
	if (m_materialPool.end() == itr) return nullptr;

	return itr->second.get();
}

Texture* RendererManager::getTexture(TextureID id)
{
	const auto& itr = m_texturePool.find(id);
	if (m_texturePool.end() == itr) return nullptr;

	return itr->second.get();
}
