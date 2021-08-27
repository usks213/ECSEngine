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
#include "D3D11Shader.h"
#include "D3D11Material.h"


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

	void setD3D11Material(const MaterialID& materialID);

	void setD3D11MaterialResource(const D3D11Material& d3dMaterial, const D3D11Shader& d3dShader);

	void setD3D11RenderBuffer(const RenderBufferID& renderBufferID);

	void setD3D11Texture(std::uint32_t slot, const TextureID& textureID, ShaderStage stage);

	void setD3D11Sampler(std::uint32_t slot, SamplerState state, ShaderStage stage);

	void setD3D11PrimitiveTopology(PrimitiveTopology topology);

	void setD3D11ShaderResourceView(std::uint32_t slot, ID3D11ShaderResourceView* srv, ShaderStage stage);

public:
	void setD3DSystemBuffer(const SHADER::SystemBuffer& systemBuffer);

	void setD3DTransformBuffer(const Matrix& mtxWorld);

	void d3dRender(const RenderBufferID& renderBufferID);

	void d3dMap(ID3D11Resource* pResource, D3D11_MAP mapType, bool mapWait, SubResource& out);

	void d3dUnmap(ID3D11Resource* pResource);

	void d3dCopyResource(ID3D11Resource* pDst, ID3D11Resource* pSrc);

public:
	void setTexture(std::uint32_t slot, const TextureID& textureID, ShaderStage stage) override;

	void setViewport(Viewport viewport) override;

	void setRenderTarget(const RenderTargetID& rtID, const DepthStencilID& dsID)override;
	void setRenderTargets(std::uint32_t num, const RenderTargetID* rtIDs, const DepthStencilID& dsID) override;

	void setRenderTarget(const RenderTargetID& rtID) override;
	void setDepthStencil(const DepthStencilID& dsID) override;

	void clearRenderTarget(const RenderTargetID& rtID) override;
	void clearDepthStencil(const DepthStencilID& dsID) override;

	void copyRenderTarget(const RenderTargetID& dstID, const RenderTargetID srcID) override;

public:
	Viewport getViewport() override { return Viewport(m_vireport); }

public:
	BufferID createBuffer(BufferDesc desc, BufferData* pData = nullptr) override;
	ShaderID createShader(ShaderDesc desc) override;
	MaterialID createMaterial(std::string name, ShaderID shaderID) override;
	MeshID createMesh(std::string name) override;
	RenderBufferID createRenderBuffer(ShaderID shaderID, MeshID meshID) override;
	TextureID createTextureFromFile(std::string filePath) override;
	RenderTargetID createRenderTarget(std::string name) override;
	DepthStencilID createDepthStencil(std::string name) override;
	BatchGroupID creatBatchGroup(MaterialID materialID, MeshID meshID) override;

	SubResource mapTexture(TextureID texID) override;
	SubResource mapVertex(RenderBufferID rdID) override;

	void unmapTexture(TextureID texID) override;
	void unmapVertex(RenderBufferID rdID) override;

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

	void imguiDebug();

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

	struct GBuffer
	{
		ComPtr<ID3D11Texture2D>				m_diffuseRT;			// �g�U���ˌ�RT + w:metallic
		ComPtr<ID3D11RenderTargetView>		m_diffuseRTV;			// �g�U���ˌ�RTV
		ComPtr<ID3D11ShaderResourceView>	m_diffuseSRV;			// �g�U���ˌ�SRV

		ComPtr<ID3D11Texture2D>				m_normalRT;				// ���[���h�@��RT + w:roughness
		ComPtr<ID3D11RenderTargetView>		m_normalRTV;			// ���[���h�@��RTV
		ComPtr<ID3D11ShaderResourceView>	m_normalSRV;			// ���[���h�@��SRV

		ComPtr<ID3D11Texture2D>				m_positionRT;			// ���[���h���WRT
		ComPtr<ID3D11RenderTargetView>		m_positionRTV;			// ���[���h���WRTV
		ComPtr<ID3D11ShaderResourceView>	m_positionSRV;			// ���[���h���WSRV
	};
	GBuffer								m_gbuffer;
	DXGI_FORMAT							m_diffuseFormat;
	DXGI_FORMAT							m_normalFormat;

	std::uint32_t						m_backBufferCount;
	int									m_nOutputWidth;
	int									m_nOutputHeight;

	ComPtr<IDXGIFactory2>				m_dxgiFactory;			// �t�@�N�g���[
	DXGI_SAMPLE_DESC					m_dxgiMSAA;				// MSAA�ݒ�
	D3D11_VIEWPORT						m_vireport;				// �r���[�|�[�g
	HWND								m_hWnd;					// �E�B���h�E�n���h��

	bool								m_bUseMSAA;

	ComPtr<ID3D11RasterizerState>		m_rasterizeStates[(size_t)RasterizeState::MAX];		// ���X�^���C�U�X�e�[�g
	ComPtr<ID3D11SamplerState>			m_samplerStates[(size_t)SamplerState::MAX];			// �T���v���X�e�[�g
	ComPtr<ID3D11BlendState>			m_blendStates[(size_t)BlendState::MAX];				// �u�����h�X�e�[�g
	ComPtr<ID3D11DepthStencilState>		m_depthStencilStates[(size_t)DepthStencilState::MAX];	// �[�x�X�e���V���X�e�[�g

private:
	BlendState			m_curBlendState;
	RasterizeState		m_curRasterizeState;
	DepthStencilState	m_curDepthStencilState;
	PrimitiveTopology	m_curPrimitiveTopology;

	D3D11Shader*		m_curD3DShader;
	MaterialID			m_curMaterial;
	SamplerState		m_curSamplerState[static_cast<size_t>(ShaderStage::MAX)][SHADER::MAX_SAMPLER_SLOT_COUNT];
	TextureID			m_curTexture[static_cast<size_t>(ShaderStage::MAX)][SHADER::MAX_TEXTURE_SLOT_COUNT];

	ID3D11RenderTargetView* m_curRTV;
	ID3D11DepthStencilView* m_curDSV;

	ComPtr<ID3D11Buffer> m_systemBuffer;
	ComPtr<ID3D11Buffer> m_transformBuffer;
	ComPtr<ID3D11Buffer> m_materialBuffer;

};
