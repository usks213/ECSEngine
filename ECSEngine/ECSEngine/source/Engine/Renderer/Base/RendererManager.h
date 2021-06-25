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
	virtual ShaderID createShader(ShaderDesc desc) = 0;
	virtual MaterialID createMaterial(std::string name, ShaderID id) = 0;
	virtual TextureID createTexture() = 0;

	Shader*	getShader(ShaderID id);
	Material* getMaterial(MaterialID id);
	Texture* getTexture(TextureID id);

protected:
	Engine* m_pEngine;


	std::unordered_map<ShaderID, std::unique_ptr<Shader>>		m_shaderPool;
	std::unordered_map<MaterialID, std::unique_ptr<Material>>	m_materialPool;
	std::unordered_map<TextureID, std::unique_ptr<Texture>>		m_texturePool;
	std::unordered_map<TextureID, std::unique_ptr<Texture>>		m_texturePool;
};

