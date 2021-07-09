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
#include <Engine/Utility/Mathf.h>


/// @brief マテリアルID
using MaterialID = std::uint32_t;
/// @brief 存在しないマテリアルID
constexpr MaterialID NONE_MATERIAL_ID = std::numeric_limits<MaterialID>::max();


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

	/// @brief Textureデータ
	struct TextureData
	{
		std::string name;
		TextureID	id;
	};

	struct SamplerData
	{
		std::string		name;
		ESamplerState	state;
	};

	/// @brief 全ステージ、スロット分のCBufferデータ
	std::array<std::unordered_map<std::uint32_t, CBuffer>,
		static_cast<size_t>(EShaderStage::MAX)>	m_cbufferData;
	/// @brief 全ステージ、スロットのバインドするテクスチャ
	std::array<std::unordered_map<std::uint32_t, TextureData>,
		static_cast<size_t>(EShaderStage::MAX)>	m_textureData;
	/// @brief 全ステージ、スロット分のサンプラステート
	std::array<std::unordered_map<std::uint32_t, SamplerData>,
		static_cast<size_t>(EShaderStage::MAX)>	m_samplerData;

	/// @brief CBuffer変数データ
	std::unordered_map<std::string, Shader::CBufferVariable> m_cbufferVariable;

public:
    /// @brief float設定
	void setFloat(const char* name, const float& data) { setData(name, &data); }

    /// @brief Vector2設定
    void setVector2(const char* name, const Vector2& data) { setData(name, &data); }

    /// @brief Vector3設定
    void setVector3(const char* name, const Vector3& data) { setData(name, &data); }

    /// @brief Vector4設定
    void setVector4(const char* name, const Vector4& data) { setData(name, &data); }

    /// @brief Matrix設定
    void setMatrix(const char* name, const Matrix& data) { setData(name, &data); }

    /// @brief Struct設定
    void setStruct(const char* name, const void* data) { setData(name, &data); }

    /// @brief テクスチャ設定
    void setTexture(const char* name, const TextureID textureID);

    /// @brief テクスチャ取得
    TextureID getTexture(const char* name);

    /// @brief サンプラ設定
    void setSampler(const char* name, const ESamplerState sampler);

    /// @brief サンプラ取得
    ESamplerState getSampler(const char* name);

	/// @brief データ取得
	void* getData(const char* name);

protected:
	/// @brief データ設定
	void setData(const char* name, const void* data);

};