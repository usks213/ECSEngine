/*****************************************************************//**
 * \file   D3D11RendererManager.h
 * \brief  DirectX11レンダラー
 * 
 * \author USAMI KOSHI
 * \date   2021/06/17
 *********************************************************************/
#pragma once

#include <Engine/Renderer/Base/RendererManager.h>
#include "D3D11Utility.h"
#include "D3D11Shader.h"
#include "D3D11Material.h"


/// @brief DirectX11レンダラー管理クラス
/// @class D3D11RendererManager
class D3D11RendererManager : public RendererManager
{
public:
	/// @brief コンストラクタ
	D3D11RendererManager();
	/// @brief デストラクタ
	~D3D11RendererManager();

	/// @brief 初期化
	/// @param hWnd ウィンドウハンドル
	/// @param width ウィンドウの幅
	/// @param height ウィンドウの高さ
	/// @return 成功 TRUE 失敗 FALSE
	HRESULT initialize(HWND hWnd, int width, int height);

	/// @brief 終了処理
	void finalize() override;

	/// @brief バッファクリア
	void clear() override;

	/// @brief スワップ
	void present() override;

public:

	void setD3D11Material(const MaterialID& materialID);

	void setD3D11MaterialResource(const D3D11Material& d3dMaterial, const D3D11Shader& d3dShader);

	void setD3D11Texture(std::uint32_t slot, const TextureID& textureID, EShaderStage stage);

	void setD3D11Sampler(std::uint32_t slot, ESamplerState state, EShaderStage stage);

	void setD3D11PrimitiveTopology(EPrimitiveTopology topology);

	void setD3DSystemBuffer(const D3D::SystemBuffer& systemBuffer);

	void setD3DTransformBuffer(const Matrix& mtxWorld);

public:
	void setTexture(std::uint32_t slot, const TextureID& textureID, EShaderStage stage) override;

	/// @brief 描画
	/// @param materialID マテリアルID
	/// @param meshID メッシュID
	void render(const MaterialID& materialID, const MeshID& meshID) override;

	ShaderID createShader(ShaderDesc desc) override;
	MaterialID createMaterial(std::string name, ShaderID shaderID) override;
	MeshID createMesh(std::string name) override;
	RenderBufferID createRenderBuffer(ShaderID shaderID, MeshID meshID) override;
	TextureID createTextureFromFile(std::string filePath) override;

private:
	/// @brief デバイスの生成
	/// @return HRESULT
	HRESULT createDivece();
	/// @brief スワップチェーンとバッファの生成
	/// @return HRESULT
	HRESULT createSwapChainAndBuffer();
	/// @brief 共通ステートの生成
	/// @return HRESULT
	HRESULT createCommonState();

	void imguiDebug();

public:
	ComPtr<ID3D11Device1>				m_d3dDevice;			// デバイス
	ComPtr<ID3D11DeviceContext1>		m_d3dContext;			// デバイスコンテキスト
	ComPtr<ID3D11DeviceContext1>		m_d3dDefferedContext;	// 遅延コンテキスト
	ComPtr<IDXGISwapChain1>				m_swapChain;			// スワップチェーン
	ComPtr<ID3DUserDefinedAnnotation>	m_d3dAnnotation;		// アノテーション

	ComPtr<ID3D11Texture2D>				m_backBufferRT;			// バックバッファ
	ComPtr<ID3D11RenderTargetView>		m_backBufferRTV;		// バックバッファビュー
	DXGI_FORMAT							m_backBufferFormat;

	ComPtr<ID3D11Texture2D>				m_diffuseRT;			// 拡散反射光RT
	ComPtr<ID3D11RenderTargetView>		m_diffuseRTV;			// 拡散反射光RTV
	ComPtr<ID3D11ShaderResourceView>	m_diffuseSRV;			// 拡散反射光SRV

	ComPtr<ID3D11Texture2D>				m_depthStencilTexture;	// Zバッファ
	ComPtr<ID3D11DepthStencilView>		m_depthStencilView;		// Zバッファビュー
	DXGI_FORMAT							m_depthStencilFormat;

	std::uint32_t						m_backBufferCount;
	int									m_nOutputWidth;
	int									m_nOutputHeight;

	ComPtr<IDXGIFactory2>				m_dxgiFactory;			// ファクトリー
	DXGI_SAMPLE_DESC					m_dxgiMSAA;				// MSAA設定
	D3D11_VIEWPORT						m_vireport;				// ビューポート
	HWND								m_hWnd;					// ウィンドウハンドル

	bool								m_bUseMSAA;

	ComPtr<ID3D11RasterizerState>		m_rasterizeStates[(size_t)ERasterizeState::MAX];		// ラスタライザステート
	ComPtr<ID3D11SamplerState>			m_samplerStates[(size_t)ESamplerState::MAX];			// サンプラステート
	ComPtr<ID3D11BlendState>			m_blendStates[(size_t)EBlendState::MAX];				// ブレンドステート
	ComPtr<ID3D11DepthStencilState>		m_depthStencilStates[(size_t)EDepthStencilState::MAX];	// 深度ステンシルステート

private:
	EBlendState			m_curBlendState;
	ERasterizeState		m_curRasterizeState;
	EDepthStencilState	m_curDepthStencilState;
	EPrimitiveTopology	m_curPrimitiveTopology;

	D3D11Shader*		m_curD3DShader;
	MaterialID			m_curMaterial;
	ESamplerState		m_curSamplerState[static_cast<size_t>(EShaderStage::MAX)][D3D::MAX_SAMPLER_SLOT_COUNT];
	TextureID			m_curTexture[static_cast<size_t>(EShaderStage::MAX)][D3D::MAX_TEXTURE_SLOT_COUNT];


	ComPtr<ID3D11Buffer> m_systemBuffer;
	ComPtr<ID3D11Buffer> m_transformBuffer;
	ComPtr<ID3D11Buffer> m_materialBuffer;

};
