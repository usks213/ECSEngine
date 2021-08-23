/*****************************************************************//**
 * \file   D3D11Texture.h
 * \brief  DirectX11テクスチャ
 * 
 * \author USAMI KOSHI
 * \date   2021/07/04
 *********************************************************************/
#pragma once

#include <Engine/Renderer/Base/Texture.h>
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;
#include <d3d11_1.h>
#include <DirectXTex.h>


/// @class D3D11Texture
/// @brief DirectX11テクスチャ
class D3D11Texture final : public Texture
{
public:
    /// @brief コンストラクタ
    explicit D3D11Texture(ID3D11Device1* device, ID3D11DeviceContext1* context,const TextureID& id, const std::string& path);

    /// @brief デストラクタ
    ~D3D11Texture() = default;

    ComPtr<ID3D11Texture2D>          m_tex; // テクスチャ
    ComPtr<ID3D11ShaderResourceView> m_srv; // シェーダリソースビュー

private:

	void CreateTextureFromFile(_In_ ID3D11Device1* d3dDevice,
		_In_ ID3D11DeviceContext1* context,
		_In_z_ const wchar_t* szFileName,
		_Out_opt_ DirectX::TexMetadata* pTexInfo = nullptr
	);

	void CreateTextureFromFile(_In_ ID3D11Device1* d3dDevice,
		_In_ ID3D11DeviceContext1* context,
		_In_z_ const char* szFileName,
		_Out_opt_ DirectX::TexMetadata* pTexInfo = nullptr
	);
};
