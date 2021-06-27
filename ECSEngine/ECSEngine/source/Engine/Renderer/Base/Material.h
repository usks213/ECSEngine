/*****************************************************************//**
 * \file   Material.h
 * \brief  マテリアル
 * 
 * \author USAMI KOSHI
 * \date   2021/06/20
 *********************************************************************/
#pragma once

#include "CommonState.h"
#include "Shader.h"
#include "Texture.h"


/// @brief マテリアルID
using MaterialID = std::uint32_t;

/// @class Material
/// @brief マテリアル
class Material
{
public:
	/// @brief コンストラクタ
	/// @param シェーダ情報
	explicit Material(const MaterialID& id, const std::string& name, const Shader& shader);

	/// @brief デストラクタ
	virtual ~Material() = default;


public:
	/// @brief マテリアルID
	MaterialID			m_id;
	/// @brief マテリアル名
	std::string			m_name;
	/// @brief 半透明フラグ
	bool				m_isTransparent;
	/// @brief ブレンドステート(半透明のみ)
	EBlendState			m_blendState;
	/// @brief 深度ステンシルステート
	EDepthStencilState	m_depthStencilState;
	/// @brief ラスタライザステート
	ERasterizeState		m_rasterizeState;
	/// @brief シェーダID
	ShaderID			m_shaderID;

	/// @brief CBufferデータ
	struct CBuffer
	{
		std::string						name;
		std::unique_ptr<std::byte[]>	data;
		std::size_t						size;
		bool							isUpdate;	//trueなら更新した
	};

	/// @brief 全ステージ、スロット分のCBufferデータ
	std::array<std::unordered_map<std::uint32_t, CBuffer>,
		static_cast<size_t>(EShaderStage::MAX)>	m_cbufferData;
	/// @brief 全ステージ、スロットのバインドするテクスチャID
	std::array<std::unordered_map<std::uint32_t, TextureID>,
		static_cast<size_t>(EShaderStage::MAX)>	m_textureData;
	/// @brief 全ステージ、スロット分のサンプラステート
	std::array<std::unordered_map<std::uint32_t, ESamplerState>,
		static_cast<size_t>(EShaderStage::MAX)>	m_samplerData;
};