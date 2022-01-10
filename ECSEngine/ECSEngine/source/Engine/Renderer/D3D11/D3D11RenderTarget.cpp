/*****************************************************************//**
 * \file   D3D11RenderTarget.cpp
 * \brief  D3D11レンダーターゲット
 * 
 * \author USAMI KOSHI
 * \date   2021/08/07
 *********************************************************************/

#include "D3D11RenderTarget.h"
#include "D3D11Utility.h"

template<typename T> static constexpr T numMipmapLevels(T width, T height)
{
	T levels = 1;
	while ((width | height) >> levels) {
		++levels;
	}
	return levels;
}

 /// @brief コンストラクタ
 /// @param device デバイス
 /// @param desc レンダーターゲットDesc
D3D11RenderTarget::D3D11RenderTarget(ID3D11Device1* device, const D3D11RenderTargetDesc& desc) :
	RenderTarget(desc.id, desc.name)
{
	// テクスチャ2D
	D3D11_TEXTURE2D_DESC renderTexture = {};
	renderTexture.Format = desc.format;
	renderTexture.Width = static_cast<UINT>(desc.width);
	renderTexture.Height = static_cast<UINT>(desc.height);
	renderTexture.ArraySize = 1;
	renderTexture.MipLevels = 1;
	renderTexture.CPUAccessFlags = 0;
	renderTexture.Usage = D3D11_USAGE_DEFAULT;
	renderTexture.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	if (desc.isMSAA)
		renderTexture.SampleDesc = desc.sampleDesc;
	else
		renderTexture.SampleDesc.Count = 1;

	if (desc.isMipMap)
	{
		renderTexture.MipLevels = numMipmapLevels(renderTexture.Width, renderTexture.Height);
		renderTexture.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
	}

	CHECK_FAILED(device->CreateTexture2D(
		&renderTexture,
		nullptr,
		m_tex.ReleaseAndGetAddressOf()
	));

	// レンダラーターゲットビュー
	CD3D11_RENDER_TARGET_VIEW_DESC rtvDesc(D3D11_RTV_DIMENSION_TEXTURE2D, desc.format);
	if (desc.isMSAA) rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;

	CHECK_FAILED(device->CreateRenderTargetView(
		m_tex.Get(),
		&rtvDesc,
		m_rtv.ReleaseAndGetAddressOf()));

	// シェーダーリソース
	CD3D11_SHADER_RESOURCE_VIEW_DESC srvDesc(D3D11_SRV_DIMENSION_TEXTURE2D, desc.format);
	//srvDesc.Texture2D.MipLevels = renderTexture.MipLevels;
	if (desc.isMSAA) srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
	CHECK_FAILED(device->CreateShaderResourceView(
		m_tex.Get(),
		&srvDesc,
		m_srv.ReleaseAndGetAddressOf()
	));
}
