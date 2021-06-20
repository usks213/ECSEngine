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
	m_shaderID(shader.m_id)
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
			m_textureData[stageIndex][tex.second.slot] = 
				std::numeric_limits<std::uint32_t>::max();
		}

		//--- サンプラ
		for (const auto& smp : shader.m_samplerBindDatas[stageIndex])
		{
			m_samplerData[stageIndex][smp.second.slot] = ESamplerState::NONE;
		}
	}
}
