/*****************************************************************//**
 * \file   D3D11Texture.h
 * \brief  DirectX11テクスチャ
 * 
 * \author USAMI KOSHI
 * \date   2021/07/04
 *********************************************************************/
#pragma once

#include <Engine/Renderer/Base/Texture.h>
#include "D3D11Utility.h"
//#include <DirectXTex.h>


class D3D11Texture final : public Texture
{
public:
    /// @brief コンストラクタ
    D3D11Texture(ID3D11Device1* device, const TextureID& id, const std::string& name);

    /// @brief デストラクタ
    ~D3D11Texture() = default;

    ComPtr<ID3D11Texture2D>          m_tex; // テクスチャ
    ComPtr<ID3D11ShaderResourceView> m_srv; // シェーダリソースビュー

private:

	/*static HRESULT CreateTextureFromFile(_In_ ID3D11Device* d3dDevice,
		_In_z_ const wchar_t* szFileName,
		_Out_opt_ ID3D11ShaderResourceView** textureView,
		_Out_opt_ DirectX::TexMetadata* pTexInfo = nullptr
	);

	static HRESULT CreateTextureFromFile(_In_ ID3D11Device* d3dDevice,
		_In_z_ const char* szFileName,
		_Out_opt_ ID3D11ShaderResourceView** textureView,
		_Out_opt_ DirectX::TexMetadata* pTexInfo = nullptr
	);*/
};
