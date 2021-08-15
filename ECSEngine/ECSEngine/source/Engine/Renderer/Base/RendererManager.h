/*****************************************************************//**
 * \file   RendererManager.h
 * \brief  レンダラーマネージャーのベースクラス
 * 
 * \author USAMI KOSHI
 * \date   2021/06/14
 *********************************************************************/
#pragma once

#include "Shader.h"
#include "Texture.h"
#include "Material.h"
#include "Mesh.h"
#include "RenderBuffer.h"
#include "BatchGroup.h"
#include "SubResource.h"
#include "RenderTarget.h"
#include "DepthStencil.h"

#include <Engine/Utility/Mathf.h>


// 前宣言
class Engine;

/// @brief レンダラーのベースクラス
/// @class RendererManager
class RendererManager
{
	friend class Engine;
public:
	RendererManager() = default;
	virtual ~RendererManager() = default;

	virtual void finalize() = 0;
	virtual void clear() = 0;
	virtual void present() = 0;


	RendererManager(const RendererManager&) = delete;
	RendererManager(RendererManager&&) = delete;

public:
	/// @brief マテリアル指定
	/// @param materialID マテリアルID
	void setMaterial(const MaterialID& materialID) {}

	/// @brief テクスチャ指定
	/// @param slot スロット
	/// @param textureID テクスチャID
	/// @param stage シェーダーステージ
	virtual void setTexture(std::uint32_t slot, const TextureID& textureID, EShaderStage stage) = 0;

	virtual void setViewport(Viewport viewport) = 0;

	virtual void setRenderTarget(const RenderTargetID& rtID, const DepthStencilID& dsID) = 0;
	virtual void setRenderTargets(std::uint32_t num, const RenderTargetID* rtIDs, const DepthStencilID& dsID) = 0;

	virtual void setRenderTarget(const RenderTargetID& rtID) = 0;
	virtual void setDepthStencil(const DepthStencilID& dsID) = 0;

	virtual void clearRenderTarget(const RenderTargetID& rtID) = 0;
	virtual void clearDepthStencil(const DepthStencilID& dsID) = 0;

	virtual void copyRenderTarget(const RenderTargetID& dstID, const RenderTargetID srcID) = 0;
public:
	/// @brief ビューポート取得
	virtual Viewport getViewport() = 0;

public:
	virtual ShaderID createShader(ShaderDesc desc) = 0;
	virtual MaterialID createMaterial(std::string name, ShaderID id) = 0;
	virtual MeshID createMesh(std::string name) = 0;
	virtual RenderBufferID createRenderBuffer(ShaderID shaderID, MeshID meshID) = 0;
	virtual TextureID createTextureFromFile(std::string filePath) = 0;
	virtual RenderTargetID createRenderTarget(std::string name) = 0;
	virtual DepthStencilID createDepthStencil(std::string name) = 0;
	virtual BatchGroupID creatBatchGroup(MaterialID materialID, MeshID meshID) = 0;

	Shader*	getShader(ShaderID id);
	Material* getMaterial(MaterialID id);
	Mesh* getMesh(MeshID id);
	RenderBuffer* getRenderBuffer(RenderBufferID id);
	Texture* getTexture(TextureID id);
	RenderTarget* getRenderTarget(RenderTargetID id);
	DepthStencil* getDepthStencil(DepthStencilID id);
	BatchGroup* getBatchGroup(BatchGroupID id);

	virtual SubResource mapTexture(TextureID texID) = 0;
	virtual SubResource mapVertex(RenderBufferID rdID) = 0;

	virtual void unmapTexture(TextureID texID) = 0;
	virtual void unmapVertex(RenderBufferID rdID) = 0;

protected:
	Engine* m_pEngine;

public:
	std::unordered_map<ShaderID,		std::unique_ptr<Shader>>		m_shaderPool;
	std::unordered_map<MaterialID,		std::unique_ptr<Material>>		m_materialPool;
	std::unordered_map<MeshID,			std::unique_ptr<Mesh>>			m_meshPool;
	std::unordered_map<RenderBufferID,	std::unique_ptr<RenderBuffer>>	m_renderBufferPool;
	std::unordered_map<TextureID,		std::unique_ptr<Texture>>		m_texturePool;
	std::unordered_map<RenderTargetID,	std::unique_ptr<RenderTarget>>	m_renderTargetPool;
	std::unordered_map<DepthStencilID,	std::unique_ptr<DepthStencil>>	m_depthStencilPool;
	std::unordered_map<BatchGroupID,	std::unique_ptr<BatchGroup>>	m_batchGroupPool;
};

