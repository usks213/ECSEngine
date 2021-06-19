/*****************************************************************//**
 * \file   D3D11Shader.h
 * \brief  Directx11�V�F�[�_
 * 
 * \author USAMI KOSHI
 * \date   2021/06/19
 *********************************************************************/
#pragma once

#include <Engine/Renderer/Base/ShaderBase.h>
#include <Engine/Renderer/Base/D3DUtility.h>
#include <d3d11_1.h>
#include <d3d11shader.h>


/// @class D3D11Shader
/// @brief DirectX11�V�F�[�_
class D3D11Shader : public ShaderBase
{
public:
	/// @brief  �R���X�g���N�^
	/// @param device �f�o�C�X
	/// @param desc �V�F�[�_���
	D3D11Shader(ID3D11Device1* device, ShaderDesc desc);
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

	/// @brief ���̓��C�A�E�g���\����
	struct InputLayoutVariable
	{
		enum class FormatType {
			R32,
			R32G32,
			R32G32B32,
			R32G32B32A32,
			MAX,
		};
		std::string semanticName;		// �Z�}���e�B�b�N��		��:TEXCOOD
		std::uint32_t semanticIndex;	// �Z�}���e�B�b�N�ԍ�	��:TEXCOOD[0]��
		FormatType formatType;			// �t�H�[�}�b�g�^�C�v	��:R32=0,R32B32=1, MAX=4
		DXGI_FORMAT	format;				// �t�H�[�}�b�g
		InputLayoutVariable():
			semanticName(),
			semanticIndex(0),
			formatType(FormatType::MAX),
			format(DXGI_FORMAT_UNKNOWN){}
	};
	/// @brief ���̓��C�A�E�g��񃊃X�g
	std::vector<InputLayoutVariable> m_inputLayoutVariableList;

	/// @brief CBuffer�̕ϐ����
	struct CBufferVariable
	{
		std::string name;	// �ϐ���
		std::size_t size;	// �^�T�C�Y
		std::size_t offset;	// �ϐ��I�t�Z�b�g
	};
	/// @brief CBuffer�̍\���̃��C�A�E�g���
	struct CBufferLayout
	{
		std::string		name;	// cbuffer�錾��
		std::uint32_t	slot;	// ���W�X�^�X���b�g
		std::size_t		size;	// cbuffer�T�C�Y
		std::vector<CBufferVariable> variables;	// �ϐ��f�[�^
		CBufferLayout(const std::uint32_t& slot, const std::string& name, const std::size_t& size) :
			slot(slot), name(name), size(size){}
	};
	/// @brief �S�X�e�[�W��CBuffer���C�A�E�g
	std::array<std::unordered_map<std::uint32_t, CBufferLayout>, 
		static_cast<size_t>(EShaderStage::MAX)> m_cbufferLayouts;
	/// @brief CBuffer�ϐ��̃f�t�H���g�l(�������q�t���l)
	std::unordered_map<std::string, std::unique_ptr<std::byte[]>> m_cbufferDefaults;

	/// @brief �e�N�X�`�����\�[�X�o�C���h���
	struct TextureBindData
	{
		std::string name;
		EShaderStage stage;
		std::uint32_t slot;
	};
	/// @brief �T���v�����\�[�X�o�C���h���
	struct SamplerBindData
	{
		std::string name;
		EShaderStage stage;
		std::uint32_t slot;
	};

	/// @brief �S�X�e�[�W�̃e�N�X�`�����\�[�X���
	std::array<std::unordered_map<std::uint32_t, TextureBindData>,
		static_cast<size_t>(EShaderStage::MAX)> m_textureBindDatas;
	/// @brief �S�X�e�[�W�̃T���v�����\�[�X���
	std::array<std::unordered_map<std::uint32_t, SamplerBindData>,
		static_cast<size_t>(EShaderStage::MAX)> m_samplerBindDatas;

private:
	/// @brief �V�F�[�_�̃X�}�[�g�|�C���^(����p)
	std::vector<ComPtr<ID3D11DeviceChild>>	m_comShaders;

private:
	/// @brief �V�F�[�_�I�u�W�F�N�g����
	/// @param device �f�o�C�X
	/// @param stage �V�F�[�_�X�e�[�W
	/// @param blob �R���p�C���f�[�^
	void createShaderObjct(ID3D11Device1* device, const EShaderStage& stage, ComPtr<ID3DBlob>& blob);

private:

};

