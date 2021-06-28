/*****************************************************************//**
 * \file   Material.cpp
 * \brief  マテリアル
 * 
 * \author USAMI KOSHI
 * \date   2021/06/20
 *********************************************************************/

#include "Material.h"


 /// @brief コンストラクタ
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
	// シェーダからマテリアルデータを生成
	for (EShaderStage stage = EShaderStage::VS; stage < EShaderStage::MAX; ++stage)
	{
		//--- CBuffer確保
		auto stageIndex = static_cast<size_t>(stage);
		for (const auto& cbLayout : shader.m_cbufferLayouts[stageIndex])
		{
			auto& cbData = m_cbufferData[stageIndex][cbLayout.first];
			// 名前
			cbData.name = cbLayout.second.name;
			// バッファデータ確保
			cbData.data = std::make_unique<std::byte[]>(cbLayout.second.size);
			// バッファサイズ
			cbData.size = cbLayout.second.size;
			// 更新フラグ
			cbData.isUpdate = true;

			// 変数データ格納
			for (const auto& var : cbLayout.second.variables)
			{
				const auto& defaltVar = shader.m_cbufferDefaults.find(var.name);
				// 初期化子データがある場合は初期化する
				if (defaltVar != shader.m_cbufferDefaults.end())
				{
					std::memcpy(cbData.data.get() + var.offset, 
						defaltVar->second.get(), var.size);	// 怪しい…
				}
				else
				{
					std::memset(cbData.data.get() + var.offset, 0, var.size);
				}
			}
		}

		//--- テクスチャ初期化
		for (const auto& tex : shader.m_textureBindDatas[stageIndex])
		{
			m_textureData[stageIndex][tex.second.slot] = NONE_TEXTURE_ID;
		}

		//--- サンプラ
		for (const auto& smp : shader.m_samplerBindDatas[stageIndex])
		{
			m_samplerData[stageIndex][smp.second.slot] = ESamplerState::NONE;
		}
	}
}

/// @brief データ設定
void Material::setData(const char* name, const void* data)
{
	// 検索
	for (EShaderStage stage = EShaderStage::VS; stage < EShaderStage::MAX; ++stage)
	{
		auto stageIndex = static_cast<size_t>(stage);
		for (const auto& cbLayout : m_pShader->m_cbufferLayouts[stageIndex])
		{
			bool bBreak = false;
			// 変数データ
			for (const auto& var : cbLayout.second.variables)
			{
				// 一致した
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

/// @brief テクスチャ設定
void Material::setTexture(const char* name, const TextureID textureID)
{
	// 検索
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

/// @brief テクスチャ取得
TextureID Material::getTexture(const char* name)
{
	// 検索
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

/// @brief サンプラ設定
void Material::setSampler(const char* name, const ESamplerState sampler)
{
	// 検索
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

/// @brief サンプラ取得
ESamplerState Material::getSampler(const char* name)
{
	// 検索
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
