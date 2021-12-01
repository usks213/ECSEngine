/*****************************************************************//**
 * \file   RendererManager.cpp
 * \brief  レンダラーマネージャーのベースクラス
 *
 * \author USAMI KOSHI
 * \date   2021/06/14
 *********************************************************************/

#include "RendererManager.h"

Animation* RendererManager::getAnimation(AnimationID id)
{
	const auto& itr = m_animationPool.find(id);
	if (m_animationPool.end() == itr) return nullptr;

	return itr->second.get();
}

Buffer* RendererManager::getBuffer(BufferID id)
{
	const auto& itr = m_bufferPool.find(id);
	if (m_bufferPool.end() == itr) return nullptr;

	return itr->second.get();
}

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

Mesh* RendererManager::getMesh(MeshID id)
{
	const auto& itr = m_meshPool.find(id);
	if (m_meshPool.end() == itr) return nullptr;

	return itr->second.get();
}

RenderBuffer* RendererManager::getRenderBuffer(RenderBufferID id)
{
	const auto& itr = m_renderBufferPool.find(id);
	if (m_renderBufferPool.end() == itr) return nullptr;

	return itr->second.get();
}

Texture* RendererManager::getTexture(TextureID id)
{
	const auto& itr = m_texturePool.find(id);
	if (m_texturePool.end() == itr) return nullptr;

	return itr->second.get();
}

RenderTarget* RendererManager::getRenderTarget(RenderTargetID id)
{
	const auto& itr = m_renderTargetPool.find(id);
	if (m_renderTargetPool.end() == itr) return nullptr;

	return itr->second.get();
}

DepthStencil* RendererManager::getDepthStencil(DepthStencilID id)
{
	const auto& itr = m_depthStencilPool.find(id);
	if (m_depthStencilPool.end() == itr) return nullptr;

	return itr->second.get();
}

BatchGroup* RendererManager::getBatchGroup(BatchGroupID id)
{
	const auto& itr = m_batchGroupPool.find(id);
	if (m_batchGroupPool.end() == itr) return nullptr;

	return itr->second.get();
}