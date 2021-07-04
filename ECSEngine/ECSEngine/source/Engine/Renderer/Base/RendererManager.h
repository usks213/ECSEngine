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

#include <Engine/Utility/Mathf.h>


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


	/// @brief 描画
	/// @param materialID マテリアルID
	/// @param meshID メッシュID
	virtual void render(const MaterialID& materialID, const MeshID& meshID) = 0;

public:
	virtual ShaderID createShader(ShaderDesc desc) = 0;
	virtual MaterialID createMaterial(std::string name, ShaderID id) = 0;
	virtual MeshID createMesh(std::string name) = 0;
	virtual RenderBufferID createRenderBuffer(ShaderID shaderID, MeshID meshID) = 0;
	virtual TextureID createTextureFromFile(std::string filePath) = 0;

	Shader*	getShader(ShaderID id);
	Material* getMaterial(MaterialID id);
	Mesh* getMesh(MeshID id);
	RenderBuffer* getRenderBuffer(RenderBufferID id);
	Texture* getTexture(TextureID id);

protected:
	Engine* m_pEngine;


	std::unordered_map<ShaderID,		std::unique_ptr<Shader>>		m_shaderPool;
	std::unordered_map<MaterialID,		std::unique_ptr<Material>>		m_materialPool;
	std::unordered_map<MeshID,			std::unique_ptr<Mesh>>			m_meshPool;
	std::unordered_map<RenderBufferID,	std::unique_ptr<RenderBuffer>>	m_renderBufferPool;
	std::unordered_map<TextureID,		std::unique_ptr<Texture>>		m_texturePool;
};

