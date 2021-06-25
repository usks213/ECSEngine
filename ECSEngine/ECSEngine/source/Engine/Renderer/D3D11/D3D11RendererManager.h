/*****************************************************************//**
 * \file   D3D11RendererManager.h
 * \brief  DirectX11�����_���[
 * 
 * \author USAMI KOSHI
 * \date   2021/06/17
 *********************************************************************/
#pragma once

#include <Engine/Renderer/Base/RendererManager.h>
#include "D3D11Utility.h"


/// @brief DirectX11�����_���[�Ǘ��N���X
/// @class D3D11RendererManager
class D3D11RendererManager : public RendererManager
{
public:
	/// @brief �R���X�g���N�^
	D3D11RendererManager();
	/// @brief �f�X�g���N�^
	~D3D11RendererManager();

	/// @brief ������
	/// @param hWnd �E�B���h�E�n���h��
	/// @param width �E�B���h�E�̕�
	/// @param height �E�B���h�E�̍���
	/// @return ���� TRUE ���s FALSE
	HRESULT initialize(HWND hWnd, int width, int height);

	/// @brief �I������
	void finalize() override;

	/// @brief �o�b�t�@�N���A
	void clear() override;

	/// @brief �X���b�v
	void present() override;

public:
	ShaderID createShader(ShaderDesc desc) override;
	MaterialID createMaterial(std::string name, ShaderID shaderID) override;
	TextureID createTexture() override;

private:
	/// @brief �f�o�C�X�̐���
	/// @return HRESULT
	HRESULT createDivece();
	/// @brief �X���b�v�`�F�[���ƃo�b�t�@�̐���
	/// @return HRESULT
	HRESULT createSwapChainAndBuffer();
	/// @brief ���ʃX�e�[�g�̐���
	/// @return HRESULT
	HRESULT createCommonState();

public:
	ComPtr<ID3D11Device1>				m_d3dDevice;			// �f�o�C�X
	ComPtr<ID3D11DeviceContext1>		m_d3dContext;			// �f�o�C�X�R���e�L�X�g
	ComPtr<ID3D11DeviceContext1>		m_d3dDefferedContext;	// �x���R���e�L�X�g
	ComPtr<IDXGISwapChain1>				m_swapChain;			// �X���b�v�`�F�[��
	ComPtr<ID3DUserDefinedAnnotation>	m_d3dAnnotation;		// �A�m�e�[�V����

	ComPtr<ID3D11Texture2D>				m_backBufferRT;			// �o�b�N�o�b�t�@
	ComPtr<ID3D11RenderTargetView>		m_backBufferRTV;		// �o�b�N�o�b�t�@�r���[
	DXGI_FORMAT							m_backBufferFormat;

	ComPtr<ID3D11Texture2D>				m_depthStencilTexture;	// Z�o�b�t�@
	ComPtr<ID3D11DepthStencilView>		m_depthStencilView;		// Z�o�b�t�@�r���[
	DXGI_FORMAT							m_depthStencilFormat;

	std::uint32_t						m_backBufferCount;
	int									m_nOutputWidth;
	int									m_nOutputHeight;

	ComPtr<IDXGIFactory2>				m_dxgiFactory;			// �t�@�N�g���[
	DXGI_SAMPLE_DESC					m_dxgiMSAA;				// MSAA�ݒ�
	D3D11_VIEWPORT						m_vireport;				// �r���[�|�[�g
	HWND								m_hWnd;					// �E�B���h�E�n���h��

	bool								m_bUseMSAA;

	ComPtr<ID3D11RasterizerState>		m_rasterizeStates[(size_t)ERasterizeState::MAX];		// ���X�^���C�U�X�e�[�g
	ComPtr<ID3D11SamplerState>			m_samplerStates[(size_t)ESamplerState::MAX];			// �T���v���X�e�[�g
	ComPtr<ID3D11BlendState>			m_blendStates[(size_t)EBlendState::MAX];				// �u�����h�X�e�[�g
	ComPtr<ID3D11DepthStencilState>		m_depthStencilStates[(size_t)EDepthStencilState::MAX];	// �[�x�X�e���V���X�e�[�g


};
