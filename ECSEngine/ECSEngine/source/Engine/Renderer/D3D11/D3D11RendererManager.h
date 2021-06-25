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
	ShaderID createShader(ShaderDesc desc) override;
	MaterialID createMaterial(std::string name, ShaderID shaderID) override;
	TextureID createTexture() override;

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

public:
	ComPtr<ID3D11Device1>				m_d3dDevice;			// デバイス
	ComPtr<ID3D11DeviceContext1>		m_d3dContext;			// デバイスコンテキスト
	ComPtr<ID3D11DeviceContext1>		m_d3dDefferedContext;	// 遅延コンテキスト
	ComPtr<IDXGISwapChain1>				m_swapChain;			// スワップチェーン
	ComPtr<ID3DUserDefinedAnnotation>	m_d3dAnnotation;		// アノテーション

	ComPtr<ID3D11Texture2D>				m_backBufferRT;			// バックバッファ
	ComPtr<ID3D11RenderTargetView>		m_backBufferRTV;		// バックバッファビュー
	DXGI_FORMAT							m_backBufferFormat;

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


};
