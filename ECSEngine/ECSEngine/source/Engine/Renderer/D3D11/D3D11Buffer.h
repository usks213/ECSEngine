/*****************************************************************//**
 * \file   D3D11Buffer.h
 * \brief  DirectX11�o�b�t�@
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
	/// @brief �R���X�g���N�^
	/// @param device �f�o�C�X
	/// @param id �o�b�t�@ID
	/// @param desc �o�b�t�@Desc
	/// @param data �������f�[�^
	D3D11Buffer(ID3D11Device1* device, const BufferID& id, const BufferDesc& desc, const BufferData* data = nullptr);

	/// @brief �f�X�g���N�^
	~D3D11Buffer() = default;

public:
	/// @brief �o�b�t�@
	ComPtr<ID3D11Buffer>				m_buffer;
	/// @brief �V�F�[�_�[���\�[�X�r���[
	ComPtr<ID3D11ShaderResourceView>	m_srv;
};

