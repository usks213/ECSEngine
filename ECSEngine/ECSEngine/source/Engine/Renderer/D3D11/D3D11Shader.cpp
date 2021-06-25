/*****************************************************************//**
 * \file   D3D11Shader.h
 * \brief  Directx11�V�F�[�_
 *
 * \author USAMI KOSHI
 * \date   2021/06/19
 *********************************************************************/

#include "D3D11Shader.h"
#include <d3dcompiler.h>
#include <fstream>
#include <iostream>

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

namespace {
	/// @brief �V�F�[�_�t�@�C���p�X
	const char* SHADER_FILE_PATH = "data/shader/";

	/// @brief �G���g���[�|�C���g�A�t�@�C�����ʎq
	const char* SHADER_TYPES[] = {
		"VS","GS","DS","HS","PS","CS",
	};
	/// @brief HLSL�o�[�W����
	const char* HLSL_VERS[] = {
		"vs_5_0",
		"gs_5_0",
		"ds_5_0",
		"hs_5_0",
		"ps_5_0",
		"cs_5_0",
	};
}

/// @brief  �R���X�g���N�^
/// @param device �f�o�C�X
/// @param desc �V�F�[�_���
D3D11Shader::D3D11Shader(ID3D11Device1* device, ShaderDesc desc) :
	Shader(desc),
	m_comShaders(static_cast<size_t>(EShaderStage::MAX)),
	m_inputLayout()
{
	// �R���p�C�������V�F�[�_�f�[�^
	std::vector<ComPtr<ID3DBlob>>				blobs(static_cast<size_t>(EShaderStage::MAX));
	// �V�F�[�_���t���N�V����
	std::vector<ComPtr<ID3D11ShaderReflection>>	reflections(static_cast<size_t>(EShaderStage::MAX));
	// �V�F�[�_���ꎞ�i�[�p
	D3D11_SHADER_DESC							shaderDesc = {};


	// �V�F�[�_�X�e�[�W�������R���p�C��������
	for (auto stage = EShaderStage::VS; stage < EShaderStage::MAX; ++stage)
	{
		// �X�e�[�W���Ȃ��ꍇ�̓X�L�b�v
		if (!hasStaderStage(desc.m_stages, stage)) continue;

		ComPtr<ID3DBlob>& blob = blobs[static_cast<size_t>(stage)];
		ComPtr<ID3D11ShaderReflection>& reflection = reflections[static_cast<size_t>(stage)];

		// �p�X
		std::string filepath = SHADER_FILE_PATH + desc.m_name + "_" + 
			SHADER_TYPES[static_cast<size_t>(stage)] + ".hlsl";
		std::wstring wfilepath = std::wstring(filepath.begin(), filepath.end());
		// �G���[
		ID3DBlob* d3dError = nullptr;
		// �X�g���[��
		std::ifstream ifsShader(filepath, std::ios::in | std::ios::binary | std::ios::ate);
		if (ifsShader.good())
		{
			// �\�[�X�t�@�C������R���p�C��
			CHECK_FAILED(D3DCompileFromFile(
				wfilepath.c_str(),
				nullptr,
				D3D_COMPILE_STANDARD_FILE_INCLUDE,
				SHADER_TYPES[static_cast<size_t>(stage)],
				HLSL_VERS[static_cast<size_t>(stage)],
				D3DCOMPILE_ENABLE_STRICTNESS,
				0,
				blob.GetAddressOf(),
				&d3dError));
			// �G���[���b�Z�[�W!!


			// �ǂݍ��ݐ���
			if (blob)
			{
				// �V�F�[�_�I�u�W�F�N�g����
				createShaderObjct(device, stage, blob);

				// �V�F�[�_���t���N�V�����擾
				void** ppBuffer = reinterpret_cast<void**>(reflection.GetAddressOf());
				CHECK_FAILED(D3DReflect(blob->GetBufferPointer(), blob->GetBufferSize(),
					IID_ID3D11ShaderReflection, ppBuffer));
			}

		}
		else
		{
			// �V�F�[�_�t�@�C���Ȃ�!!

		}
	}

	// ���_�V�F�[�_������ꍇ�̓C���v�b�g���C�A�E�g���쐬
	auto& vsReflection = reflections[static_cast<size_t>(EShaderStage::VS)];
	if (vsReflection)
	{
		auto& vsBlob = blobs[static_cast<size_t>(EShaderStage::VS)];
		vsReflection->GetDesc(&shaderDesc);
		std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayouts(shaderDesc.InputParameters);

		// ���̓��C�A�E�g����
		m_inputLayoutVariableList.resize(shaderDesc.InputParameters);
		for (UINT i = 0; i < shaderDesc.InputParameters; ++i)
		{
			D3D11_SIGNATURE_PARAMETER_DESC signature;
			vsReflection->GetInputParameterDesc(i, &signature);

			// �C���v�b�g���C�A�E�g�v�f
			D3D11_INPUT_ELEMENT_DESC inputDesc;
			inputDesc.SemanticName = signature.SemanticName;
			inputDesc.SemanticIndex = signature.SemanticIndex;
			//inputDesc.Format = ��..
			inputDesc.InputSlot = 0;
			inputDesc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
			inputDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			inputDesc.InstanceDataStepRate = 0;

			// ���̓��C�A�E�g���
			m_inputLayoutVariableList[i].semanticName = inputDesc.SemanticName;
			m_inputLayoutVariableList[i].semanticIndex = inputDesc.SemanticIndex;
			if (i > 0)
			{
				m_inputLayoutVariableList[i].offset =
					static_cast<std::size_t>(m_inputLayoutVariableList[i - 1].formatSize) * sizeof(float) +
					m_inputLayoutVariableList[i - 1].offset;
			}

			// �r�b�g�}�X�N�Ńt�H�[�}�b�g�𕪂���
			if (signature.Mask == 0x01) {
				switch (signature.ComponentType) {
				case D3D_REGISTER_COMPONENT_UINT32 : inputDesc.Format = DXGI_FORMAT_R32_UINT;  break;
				case D3D_REGISTER_COMPONENT_SINT32 : inputDesc.Format = DXGI_FORMAT_R32_SINT;  break;
				case D3D_REGISTER_COMPONENT_FLOAT32: inputDesc.Format = DXGI_FORMAT_R32_FLOAT; break;
				}
				m_inputLayoutVariableList[i].formatSize = InputLayoutVariable::FormatSize::R32;
				m_inputLayoutVariableList[i].formatType =
					static_cast<InputLayoutVariable::FormatType>(signature.ComponentType);
				
			}
			else if (signature.Mask <= 0x03) {
				switch (signature.ComponentType) {
				case D3D_REGISTER_COMPONENT_UINT32: inputDesc.Format = DXGI_FORMAT_R32G32_UINT;  break;
				case D3D_REGISTER_COMPONENT_SINT32: inputDesc.Format = DXGI_FORMAT_R32G32_SINT;  break;
				case D3D_REGISTER_COMPONENT_FLOAT32: inputDesc.Format = DXGI_FORMAT_R32G32_FLOAT; break;
				}
				m_inputLayoutVariableList[i].formatSize = InputLayoutVariable::FormatSize::R32G32;
				m_inputLayoutVariableList[i].formatType =
					static_cast<InputLayoutVariable::FormatType>(signature.ComponentType);
			}
			else if (signature.Mask <= 0x07) {
				switch (signature.ComponentType) {
				case D3D_REGISTER_COMPONENT_UINT32: inputDesc.Format = DXGI_FORMAT_R32G32B32_UINT;  break;
				case D3D_REGISTER_COMPONENT_SINT32: inputDesc.Format = DXGI_FORMAT_R32G32B32_SINT;  break;
				case D3D_REGISTER_COMPONENT_FLOAT32: inputDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT; break;
				}
				m_inputLayoutVariableList[i].formatSize = InputLayoutVariable::FormatSize::R32G32B32;
				m_inputLayoutVariableList[i].formatType =
					static_cast<InputLayoutVariable::FormatType>(signature.ComponentType);
			}
			else if (signature.Mask <= 0x0F) {
				switch (signature.ComponentType) {
				case D3D_REGISTER_COMPONENT_UINT32: inputDesc.Format = DXGI_FORMAT_R32G32B32A32_UINT;  break;
				case D3D_REGISTER_COMPONENT_SINT32: inputDesc.Format = DXGI_FORMAT_R32G32B32A32_SINT;  break;
				case D3D_REGISTER_COMPONENT_FLOAT32: inputDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; break;
				}
				m_inputLayoutVariableList[i].formatSize = InputLayoutVariable::FormatSize::R32G32B32A32;
				m_inputLayoutVariableList[i].formatType =
					static_cast<InputLayoutVariable::FormatType>(signature.ComponentType);
			}

			// ���̓��C�A�E�g�쐬�p���i�[
			inputLayouts[i] = inputDesc;
		}

		// ���̓��C�A�E�g���쐬
		if (!inputLayouts.empty()){
			CHECK_FAILED(device->CreateInputLayout(
				inputLayouts.data(),
				shaderDesc.InputParameters,
				vsBlob->GetBufferPointer(),
				vsBlob->GetBufferSize(),
				m_inputLayout.GetAddressOf()));
		}
	}

	// �R���X�^���g�E�e�N�X�`���E�T���v���̍쐬
	D3D11_SHADER_BUFFER_DESC shaderBufferDesc = {};
	for (auto stage = EShaderStage::VS; stage < EShaderStage::MAX; ++stage)
	{
		const auto& stageIndex = static_cast<size_t>(stage);
		const auto& reflection = reflections[stageIndex];
		if (!reflection) continue;
		// �V�F�[�_���擾
		reflection->GetDesc(&shaderDesc);

		// ���t���N�V��������R���X�^���g�o�b�t�@�ǂݍ���
		std::uint32_t slotOffset = 0;
		m_cbufferLayouts[stageIndex].reserve(shaderDesc.ConstantBuffers);
		for (std::uint32_t cbIdx = 0; cbIdx < shaderDesc.ConstantBuffers; ++cbIdx)
		{
			auto* constantBuffer = reflection->GetConstantBufferByIndex(cbIdx);
			constantBuffer->GetDesc(&shaderBufferDesc);
			// ���ʂ̒萔�o�b�t�@�͔�΂�??
			//if()

			// ���C�A�E�g����
			CBufferLayout cbLayout(cbIdx, shaderBufferDesc.Name, shaderBufferDesc.Size);
			cbLayout.variables.resize(shaderBufferDesc.Variables);

			// CB�ϐ��̃��C�A�E�g�쐬
			D3D11_SHADER_VARIABLE_DESC varDesc;
			for (std::uint32_t v = 0; v < shaderBufferDesc.Variables; ++v)
			{
				// �ϐ����擾
				auto* variable = constantBuffer->GetVariableByIndex(v);
				variable->GetDesc(&varDesc);
				// �ϐ������
				cbLayout.variables[v].name = varDesc.Name;
				cbLayout.variables[v].size = varDesc.Size;
				cbLayout.variables[v].offset = varDesc.StartOffset;
				// �f�t�H���g�l������ꍇ
				if (varDesc.DefaultValue != nullptr)
				{
					std::unique_ptr<std::byte[]> defaultValue = 
						std::make_unique<std::byte[]>(varDesc.Size);
					std::memcpy(defaultValue.get(), varDesc.DefaultValue, varDesc.Size);
					m_cbufferDefaults[varDesc.Name] = std::move(defaultValue);
				}
			}
			// �R���X�^���g�o�b�t�@���C�A�E�g�i�[
			m_cbufferLayouts[stageIndex].emplace(cbIdx, cbLayout);
		}


		// �e�N�X�`���E�T���v���̃��t���N�V����
		for (std::uint32_t i = 0; i < shaderDesc.BoundResources; ++i)
		{
			// �o�C���h���擾
			D3D11_SHADER_INPUT_BIND_DESC shaderInputBindDesc;
			reflection->GetResourceBindingDesc(i, &shaderInputBindDesc);
			// ���ʃ��\�[�X�̓X�L�b�v
			//if() ... SkyBox,ShadowMap,

			switch (shaderBufferDesc.Type) 
			{
			case D3D_SIT_TEXTURE:
				m_textureBindDatas[stageIndex][shaderInputBindDesc.BindPoint].name = shaderInputBindDesc.Name;
				m_textureBindDatas[stageIndex][shaderInputBindDesc.BindPoint].slot = shaderInputBindDesc.BindPoint;
				m_textureBindDatas[stageIndex][shaderInputBindDesc.BindPoint].stage = stage;
				break;

			case D3D_SIT_SAMPLER:
				m_samplerBindDatas[stageIndex][shaderInputBindDesc.BindPoint].name = shaderInputBindDesc.Name;
				m_samplerBindDatas[stageIndex][shaderInputBindDesc.BindPoint].slot = shaderInputBindDesc.BindPoint;
				m_samplerBindDatas[stageIndex][shaderInputBindDesc.BindPoint].stage = stage;
				break;

			// �F�X����c
			case D3D_SIT_UAV_RWTYPED:
				break;
			case D3D_SIT_STRUCTURED:
				break;
			case D3D_SIT_TBUFFER:
				break;
			case D3D_SIT_CBUFFER:
				break;

			default:
				// �G���[���O
				break;
			}
		}
	}
}

/// @brief �V�F�[�_�I�u�W�F�N�g����
/// @param device �f�o�C�X
/// @param stage �V�F�[�_�X�e�[�W
/// @param blob �R���p�C���f�[�^
void D3D11Shader::createShaderObjct(ID3D11Device1* device, const EShaderStage& stage, ComPtr<ID3DBlob>& blob)
{
	// com�Q��
	auto& shader = m_comShaders[static_cast<size_t>(stage)];

	// �V�F�[�_��ʐ���
	switch (stage)
	{
	case EShaderStage::VS:
	{
		ID3D11VertexShader* d3d11Shader;
		CHECK_FAILED(device->CreateVertexShader(blob->GetBufferPointer(), 
			blob->GetBufferSize(), nullptr, &d3d11Shader));
		// �C���^�[�t�F�[�X����(�Q�ƃJ�E���g����)
		d3d11Shader->QueryInterface(__uuidof(ID3D11DeviceChild), 
			reinterpret_cast<void**>(static_cast<ID3D11DeviceChild**>(&shader)));
		// �Q�ƃJ�E���g�����炷
		d3d11Shader->Release();
		vs = d3d11Shader;
	}
		break;
	case EShaderStage::GS:
	{
		ID3D11GeometryShader* d3d11Shader;
		CHECK_FAILED(device->CreateGeometryShader(blob->GetBufferPointer(),
			blob->GetBufferSize(), nullptr, &d3d11Shader));
		// �C���^�[�t�F�[�X����(�Q�ƃJ�E���g����)
		d3d11Shader->QueryInterface(__uuidof(ID3D11DeviceChild),
			reinterpret_cast<void**>(static_cast<ID3D11DeviceChild**>(&shader)));
		// �Q�ƃJ�E���g�����炷
		d3d11Shader->Release();
		gs = d3d11Shader;
	}
		break;
	case EShaderStage::DS:
	{
		ID3D11DomainShader* d3d11Shader;
		CHECK_FAILED(device->CreateDomainShader(blob->GetBufferPointer(),
			blob->GetBufferSize(), nullptr, &d3d11Shader));
		// �C���^�[�t�F�[�X����(�Q�ƃJ�E���g����)
		d3d11Shader->QueryInterface(__uuidof(ID3D11DeviceChild),
			reinterpret_cast<void**>(static_cast<ID3D11DeviceChild**>(&shader)));
		// �Q�ƃJ�E���g�����炷
		d3d11Shader->Release();
		ds = d3d11Shader;
	}
		break;
	case EShaderStage::HS:
	{
		ID3D11HullShader* d3d11Shader;
		CHECK_FAILED(device->CreateHullShader(blob->GetBufferPointer(),
			blob->GetBufferSize(), nullptr, &d3d11Shader));
		// �C���^�[�t�F�[�X����(�Q�ƃJ�E���g����)
		d3d11Shader->QueryInterface(__uuidof(ID3D11DeviceChild),
			reinterpret_cast<void**>(static_cast<ID3D11DeviceChild**>(&shader)));
		// �Q�ƃJ�E���g�����炷
		d3d11Shader->Release();
		hs = d3d11Shader;
	}
		break;
	case EShaderStage::PS:
	{
		ID3D11PixelShader* d3d11Shader;
		CHECK_FAILED(device->CreatePixelShader(blob->GetBufferPointer(),
			blob->GetBufferSize(), nullptr, &d3d11Shader));
		// �C���^�[�t�F�[�X����(�Q�ƃJ�E���g����)
		d3d11Shader->QueryInterface(__uuidof(ID3D11DeviceChild),
			reinterpret_cast<void**>(static_cast<ID3D11DeviceChild**>(&shader)));
		// �Q�ƃJ�E���g�����炷
		d3d11Shader->Release();
		ps = d3d11Shader;
	}
		break;
	case EShaderStage::CS:
	{
		ID3D11ComputeShader* d3d11Shader;
		CHECK_FAILED(device->CreateComputeShader(blob->GetBufferPointer(),
			blob->GetBufferSize(), nullptr, &d3d11Shader));
		// �C���^�[�t�F�[�X����(�Q�ƃJ�E���g����)
		d3d11Shader->QueryInterface(__uuidof(ID3D11DeviceChild),
			reinterpret_cast<void**>(static_cast<ID3D11DeviceChild**>(&shader)));
		// �Q�ƃJ�E���g�����炷
		d3d11Shader->Release();
		cs = d3d11Shader;
	}
		break;
	}
}