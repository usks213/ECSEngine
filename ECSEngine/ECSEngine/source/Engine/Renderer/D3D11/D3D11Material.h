/*****************************************************************//**
 * \file   D3D11Material.h
 * \brief  DirectX11マテリアル
 * 
 * \author USAMI KOSHI
 * \date   2021/06/21
 *********************************************************************/
#pragma once

#include <Engine/Renderer/Base/Material.h>
#include "D3D11Utility.h"


/// @class D3D11Material
/// @brief Directx11マテリアル
class D3D11Material final :public Material
{
public:
	/// @brief コンストラクタ
	/// @param device デバイス
	/// @param id マテリアルID
	/// @param name マテリアル名
	/// @param shader シェーダ
	explicit D3D11Material(ID3D11Device1* device, const MaterialID& id, 
		const std::string& name, const Shader& shader);

	/// @brief デストラクタ
	~D3D11Material() = default;

public:

	/// @brief 全ステージ、スロット分のCBufferポインタ
	std::array<std::unordered_map<std::uint32_t, ComPtr<ID3D11Buffer>>,
		static_cast<size_t>(EShaderStage::MAX)>	m_d3dCbuffer;
};

