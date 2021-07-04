/*****************************************************************//**
 * \file   D3D11Texture.cpp
 * \brief  DirectX11テクスチャ
 * 
 * \author USAMI KOSHI
 * \date   2021/07/04
 *********************************************************************/

#include "D3D11Texture.h"
#pragma comment(lib, "DirectXTex")

#include <DDSTextureLoader11.h>
#include <WICTextureLoader11.h>
//#include <DirectXTexEXR.h>


template<typename T> static constexpr T numMipmapLevels(T width, T height)
{
	T levels = 1;
	while ((width | height) >> levels) {
		++levels;
	}
	return levels;
}


 /// @brief コンストラクタ
D3D11Texture::D3D11Texture(ID3D11Device1* device, ID3D11DeviceContext1* context, const TextureID& id, const std::string& path)
	: Texture(id, path)
{
	// テクスチャをファイルから生成
	CreateTextureFromFile(device, context, path.c_str());
}


 // ワイド文字列
void D3D11Texture::CreateTextureFromFile(_In_ ID3D11Device1* d3dDevice,
	_In_ ID3D11DeviceContext1* context,
	_In_z_ const wchar_t* szFileName,
	_Out_opt_ TexMetadata* pTexInfo)
{
	TexMetadata meta;
	ScratchImage image;
	WCHAR wszExt[_MAX_EXT];
	_wsplitpath(szFileName, nullptr, nullptr, nullptr, wszExt);

	D3D11_TEXTURE2D_DESC d3d11Desc = {};
	d3d11Desc.SampleDesc.Count = 1;
	d3d11Desc.SampleDesc.Quality = 0;
	d3d11Desc.Usage = D3D11_USAGE_DEFAULT;
	d3d11Desc.CPUAccessFlags = 0;
	d3d11Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	//if (isCubeMap == 0) {
	//	//d3d11Desc.BindFlags |= D3D11_BIND_RENDER_TARGET;
	//	//d3d11Desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
	//}
	//else
	//{
	//	d3d11Desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
	//}

	if (_wcsicmp(wszExt, L".tga") == 0)
	{
		CHECK_FAILED(LoadFromTGAFile(szFileName, &meta, image));
		if (pTexInfo) *pTexInfo = meta;

		d3d11Desc.Width = meta.width;
		d3d11Desc.Height = meta.height;
		d3d11Desc.MipLevels = meta.mipLevels;
		d3d11Desc.ArraySize = meta.arraySize;
		d3d11Desc.Format = meta.format;

		D3D11_SUBRESOURCE_DATA d3d11SubresourceData = {};
		d3d11SubresourceData.pSysMem = image.GetPixels();
		d3d11SubresourceData.SysMemPitch = static_cast<UINT>(meta.width * 4);
		d3d11SubresourceData.SysMemSlicePitch = static_cast<UINT>(meta.width * meta.height * 4);

		D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
		SRVDesc.Format = d3d11Desc.Format;
		SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		//if (isCubeMap)
		//{
		//	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		//	SRVDesc.TextureCube.MostDetailedMip = 0;
		//	SRVDesc.TextureCube.MipLevels = 1;
		//}

		CHECK_FAILED(d3dDevice->CreateTexture2D(&d3d11Desc, &d3d11SubresourceData, m_tex.GetAddressOf()));
		CHECK_FAILED(d3dDevice->CreateShaderResourceView(m_tex.Get(), &SRVDesc, m_srv.GetAddressOf()));
	}
	else if (_wcsicmp(wszExt, L".dds") == 0)
	{
		//hr = LoadFromDDSFile(szFileName, DDS_FLAGS_FORCE_RGB, &meta, image);
		CHECK_FAILED(DirectX::CreateDDSTextureFromFileEx(
			d3dDevice, szFileName, 0,
			d3d11Desc.Usage, d3d11Desc.BindFlags, d3d11Desc.CPUAccessFlags, d3d11Desc.MiscFlags,
			false, reinterpret_cast<ID3D11Resource**>(m_tex.GetAddressOf()),
			m_srv.GetAddressOf()));
	}
	else if (_wcsicmp(wszExt, L".hdr") == 0)
	{
		 LoadFromHDRFile(szFileName, &meta, image);
		 //DirectX::CreateTexture();

		 //meta.mipLevels = 0;
		 //if (meta.mipLevels == 0) {
			 d3d11Desc.BindFlags |= D3D11_BIND_RENDER_TARGET;
			 d3d11Desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
		 //}

		 d3d11Desc.Width = meta.width;
		 d3d11Desc.Height = meta.height;
		 d3d11Desc.MipLevels = numMipmapLevels(meta.width, meta.height);
		 d3d11Desc.ArraySize = meta.arraySize;
		 d3d11Desc.Format = meta.format;

		 D3D11_SUBRESOURCE_DATA d3d11SubresourceData = {};
		 d3d11SubresourceData.pSysMem = image.GetPixels();
		 d3d11SubresourceData.SysMemPitch = static_cast<UINT>(meta.width * 4 * 4);
		 d3d11SubresourceData.SysMemSlicePitch = static_cast<UINT>(meta.width * meta.height * 4 * 4);

		 D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
		 SRVDesc.Format = d3d11Desc.Format;
		 SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		 SRVDesc.Texture2D.MostDetailedMip = 0;
		 SRVDesc.Texture2D.MipLevels = d3d11Desc.MipLevels;
		 
		 CHECK_FAILED(d3dDevice->CreateTexture2D(&d3d11Desc, nullptr, m_tex.GetAddressOf()));
		 CHECK_FAILED(d3dDevice->CreateShaderResourceView(m_tex.Get(), &SRVDesc, m_srv.GetAddressOf()));

		 context->UpdateSubresource(m_tex.Get(), 0, nullptr, d3d11SubresourceData.pSysMem, d3d11SubresourceData.SysMemPitch, d3d11SubresourceData.SysMemSlicePitch);
		 context->GenerateMips(m_srv.Get());

		 //CreateShaderResourceView(d3dDevice, image.GetImages(), image.GetImageCount(), meta, m_srv.GetAddressOf());
	}
	else
	{
		CHECK_FAILED(DirectX::CreateWICTextureFromFileEx(
			d3dDevice,
			szFileName,
			0,
			d3d11Desc.Usage,
			d3d11Desc.BindFlags,
			d3d11Desc.CPUAccessFlags,
			d3d11Desc.MiscFlags,
			WIC_LOADER_DEFAULT,
			reinterpret_cast<ID3D11Resource**>(m_tex.GetAddressOf()),
			m_srv.GetAddressOf()));
	}
}


// マルチバイト文字
void D3D11Texture::CreateTextureFromFile(_In_ ID3D11Device1* d3dDevice,
	_In_ ID3D11DeviceContext1* context,
	_In_z_ const char* szFileName,
	_Out_opt_ TexMetadata* pTexInfo)
{
	WCHAR wszTexFName[_MAX_PATH];
	int nLen = MultiByteToWideChar(CP_ACP, 0, szFileName, lstrlenA(szFileName), wszTexFName, _countof(wszTexFName));
	if (nLen <= 0) return;
	wszTexFName[nLen] = L'\0';
	CreateTextureFromFile(d3dDevice, context, wszTexFName, pTexInfo);
}
