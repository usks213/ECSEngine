/*****************************************************************//**
 * \file   D3D11DepthStencil.h
 * \brief  DirectX11深度ステンシル
 * 
 * \author USAMI KOSHI
 * \date   2021/08/07
 *********************************************************************/
#pragma once

#include <Engine/Renderer/Base/DepthStencil.h>
#include "D3D11Utility.h"


 /// @brief DreictX11深度ステンシルDesc
struct D3D11DepthStencilDesc
{
	DepthStencilID id;
	std::string name;
	float width;
	float height;
	DXGI_FORMAT format;
	bool isMSAA;
	DXGI_SAMPLE_DESC sampleDesc;
	D3D11DepthStencilDesc() :
		id(NONE_DEPTH_STENCIL_ID),
		name(),
		width(0.0f),
		height(0.0f),
		format(DXGI_FORMAT_D32_FLOAT),
		isMSAA(false),
		sampleDesc({ 1,0 })
	{}
};


/// @class D3D11DepthStencil
/// @brief DreictX11深度ステンシル
class D3D11DepthStencil : public DepthStencil
{
public:
	/// @brief コンストラクタ
	/// @param device デバイス
	/// @param desc 深度ステンシルDesc
	explicit D3D11DepthStencil(ID3D11Device1* device, const D3D11DepthStencilDesc& desc);

	/// @brief デストラクタ
	~D3D11DepthStencil() = default;

public:
	/// @brief D3D11テクスチャ
	ComPtr<ID3D11Texture2D>				m_tex;
	/// @brief D3D11シェーダーリソースビュー
	ComPtr<ID3D11ShaderResourceView>	m_srv;
	/// @brief D3D11深度ステンシルビュー
	ComPtr<ID3D11DepthStencilView>		m_dsv;
};
