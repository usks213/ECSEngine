/*****************************************************************//**
 * \file   D3D11Buffer.h
 * \brief  DirectX11バッファ
 * 
 * \author USAMI KOSHI
 * \date   2021/08/24
 *********************************************************************/
#pragma once

#include <Engine/Renderer/Base/Buffer.h>
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;
#include <d3d11_1.h>


class D3D11Buffer final : public Buffer
{
public:
	/// @brief コンストラクタ
	/// @param device デバイス
	/// @param id バッファID
	/// @param desc バッファDesc
	/// @param data 初期化データ
	D3D11Buffer(ID3D11Device1* device, const BufferID& id, const BufferDesc& desc, const BufferData* data = nullptr);

	/// @brief デストラクタ
	~D3D11Buffer() = default;

public:
	/// @brief バッファ
	ComPtr<ID3D11Buffer>				m_buffer;
	/// @brief シェーダーリソースビュー
	ComPtr<ID3D11ShaderResourceView>	m_srv;
};

