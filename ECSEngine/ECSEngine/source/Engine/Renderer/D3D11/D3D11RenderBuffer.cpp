/*****************************************************************//**
 * \file   D3D11RenderBuffer.h
 * \brief  DirectX11描画データバッファ
 *
 * \author USAMI KOSHI
 * \date   2021/06/25
 *********************************************************************/

#include "D3D11RenderBuffer.h"


D3D11RenderBuffer::D3D11RenderBuffer(ID3D11Device1* device, const RenderBufferID id, 
	const Shader& shader, const Mesh& mesh) :
	RenderBuffer(id, shader, mesh)
{
	D3D11_BUFFER_DESC desc = {};
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA initData = {};

	// 頂点バッファの生成
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.ByteWidth = m_vertexData.size * m_vertexData.count;
	initData.pSysMem = m_vertexData.buffer.get();
	CHECK_FAILED(device->CreateBuffer(&desc, &initData, m_vertexBuffer.GetAddressOf()));

	// インデックスの生成
	if (m_indexData.size > 0 && m_indexData.count > 0)
	{
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		desc.ByteWidth = m_indexData.size * m_indexData.count;
		initData.pSysMem = mesh.m_indexData.data();
		CHECK_FAILED(device->CreateBuffer(&desc, &initData, m_indexBuffer.GetAddressOf()));
	}
}
