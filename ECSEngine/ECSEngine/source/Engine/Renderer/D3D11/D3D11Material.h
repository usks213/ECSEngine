/*****************************************************************//**
 * \file   D3D11Material.h
 * \brief  DirectX11�}�e���A��
 * 
 * \author USAMI KOSHI
 * \date   2021/06/21
 *********************************************************************/
#pragma once

#include <Engine/Renderer/Base/Material.h>
#include "D3D11Utility.h"


/// @class D3D11Material
/// @brief Directx11�}�e���A��
class D3D11Material final :public Material
{
public:
	/// @brief �R���X�g���N�^
	/// @param device �f�o�C�X
	/// @param id �}�e���A��ID
	/// @param name �}�e���A����
	/// @param shader �V�F�[�_
	explicit D3D11Material(ID3D11Device1* device, const MaterialID& id, 
		const std::string& name, const Shader& shader);

	/// @brief �f�X�g���N�^
	~D3D11Material() = default;

public:

	/// @brief �S�X�e�[�W�A�X���b�g����CBuffer�|�C���^
	std::array<std::unordered_map<std::uint32_t, ComPtr<ID3D11Buffer>>,
		static_cast<size_t>(EShaderStage::MAX)>	m_d3dCbuffer;
};

