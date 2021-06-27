/*****************************************************************//**
 * \file   D3D11RenderBuffer.h
 * \brief  DirectX11描画データバッファ
 * 
 * \author USAMI KOSHI
 * \date   2021/06/25
 *********************************************************************/
#pragma once

#include <Engine/Renderer/Base/RenderBuffer.h>
#include <Engine/Renderer/D3D11/D3D11Utility.h>

class D3D11RenderBuffer : public RenderBuffer
{
public:

	explicit D3D11RenderBuffer(ID3D11Device1* device, const Shader& shader, const Mesh& mesh);

	~D3D11RenderBuffer() = default;

public:
	ComPtr<ID3D11Buffer> m_vertexBuffer;
	ComPtr<ID3D11Buffer> m_indexBuffer;
};

