/*****************************************************************//**
 * \file   D3D11Texture.h
 * \brief  DirectX11�e�N�X�`��
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
/// @brief DirectX11�e�N�X�`��
class D3D11Texture final : public Texture
{
public:
    /// @brief �R���X�g���N�^
    explicit D3D11Texture(const TextureID& id, const std::string& path);

    /// @brief �f�X�g���N�^
    ~D3D11Texture() = default;

	/// @brief �t�@�C������e�N�X�`���̓ǂݍ���
	HRESULT CreateFromFile(ID3D11Device1* device, ID3D11DeviceContext1* context);
	/// @brief ����������e�N�X�`���̓ǂݍ���
	HRESULT CreateFromMemory(ID3D11Device1* device, const uint8_t* wicData, size_t wicDataSize);

    ComPtr<ID3D11Texture2D>          m_tex; // �e�N�X�`��
    ComPtr<ID3D11ShaderResourceView> m_srv; // �V�F�[�_���\�[�X�r���[

private:

	HRESULT CreateTextureFromFile(_In_ ID3D11Device1* d3dDevice,
		_In_ ID3D11DeviceContext1* context,
		_In_z_ const wchar_t* szFileName,
		_Out_opt_ DirectX::TexMetadata* pTexInfo = nullptr
	);

	HRESULT CreateTextureFromFile(_In_ ID3D11Device1* d3dDevice,
		_In_ ID3D11DeviceContext1* context,
		_In_z_ const char* szFileName,
		_Out_opt_ DirectX::TexMetadata* pTexInfo = nullptr
	);

	HRESULT CreateTextureFromMemory(_In_ ID3D11Device1* d3dDevice,
		_In_bytecount_(wicDataSize) const uint8_t* wicData,
		_In_ size_t wicDataSize,
		_Out_opt_ DirectX::TexMetadata* pTexInfo = nullptr
	);
};
