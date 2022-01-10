/*****************************************************************//**
 * \file   D3D11RenderTarget.h
 * \brief  DirectX11レンダーターゲット
 * 
 * \author USAMI KOSHI
 * \date   2021/08/07
 *********************************************************************/
#pragma once

#include <Engine/Renderer/Base/RenderTarget.h>
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;
#include <d3d11_1.h>

/// @brief DreictX11レンダーターゲットDesc
struct D3D11RenderTargetDesc
{
	RenderTargetID id;
	std::string name;
	float width;
	float height;
	DXGI_FORMAT format;
	bool isMSAA;
	DXGI_SAMPLE_DESC sampleDesc;
	bool isMipMap;
	D3D11RenderTargetDesc():
		id(NONE_RENDER_TARGET_ID),
		name(),
		width(0.0f),
		height(0.0f),
		format(DXGI_FORMAT_B8G8R8A8_UNORM),
		isMSAA(false),
		sampleDesc({1,0}),
		isMipMap(false)
	{}
};

class D3D11RenderTarget final : public RenderTarget
{
public:
	/// @brief コンストラクタ
	/// @param device デバイス
	/// @param desc レンダーターゲットDesc
	explicit D3D11RenderTarget(ID3D11Device1* device, const D3D11RenderTargetDesc& desc);
	/// @brief デストラクタ
	~D3D11RenderTarget() = default;
	
public:
	/// @brief D3D11テクスチャ
	ComPtr<ID3D11Texture2D>				m_tex;
	/// @brief D3D11シェーダーリソースビュー
	ComPtr<ID3D11ShaderResourceView>	m_srv;
	/// @brief D3D11レンダーターゲットビュー
	ComPtr<ID3D11RenderTargetView>		m_rtv;
};
