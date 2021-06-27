/*****************************************************************//**
 * \file   D3D11Shader.h
 * \brief  Directx11�V�F�[�_
 * 
 * \author USAMI KOSHI
 * \date   2021/06/19
 *********************************************************************/
#pragma once

#include <Engine/Renderer/Base/Shader.h>
#include "D3D11Utility.h"
#include <d3d11shader.h>


/// @class D3D11Shader
/// @brief DirectX11�V�F�[�_
class D3D11Shader final : public Shader
{
public:
	/// @brief  �R���X�g���N�^
	/// @param device �f�o�C�X
	/// @param desc �V�F�[�_���
	explicit D3D11Shader(ID3D11Device1* device, ShaderDesc desc, ShaderID id);
	/// @brief �f�X�g���N�^
	~D3D11Shader() = default;

public:
	/// @brief �V�F�[�_�f�[�^
	union {
		struct {
			ID3D11VertexShader*		vs;
			ID3D11GeometryShader*	gs;
			ID3D11DomainShader*		ds;
			ID3D11HullShader*		hs;
			ID3D11PixelShader*		ps;
			ID3D11ComputeShader*	cs;
		};
		ID3D11DeviceChild* shaders[static_cast<size_t>(EShaderStage::MAX)];
	};
	/// @brief ���̓��C�A�E�g
	ComPtr<ID3D11InputLayout>				m_inputLayout;

private:
	/// @brief �V�F�[�_�̃X�}�[�g�|�C���^(����p)
	std::vector<ComPtr<ID3D11DeviceChild>>	m_comShaders;

private:
	/// @brief �V�F�[�_�I�u�W�F�N�g����
	/// @param device �f�o�C�X
	/// @param stage �V�F�[�_�X�e�[�W
	/// @param blob �R���p�C���f�[�^
	void createShaderObjct(ID3D11Device1* device, const EShaderStage& stage, ComPtr<ID3DBlob>& blob);

};

