/*****************************************************************//**
 * \file   D3D11RendererManager.h
 * \brief  DirectX11レンダラー
 *
 * \author USAMI KOSHI
 * \date   2021/06/17
 *********************************************************************/

#include "D3D11RendererManager.h"
#include <vector>
#include <stdio.h>
#include <functional>

#include <Engine/Engine.h>
#include <Engine/Utility/HashUtil.h>

#include "D3D11Shader.h"
#include "D3D11Material.h"
#include "D3D11RenderBuffer.h"


// ライブラリリンク
#pragma comment(lib, "D3D11.lib")
#pragma comment(lib, "DXGI.lib")

#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif


namespace {
	static std::function<void(ID3D11DeviceContext1*, UINT, UINT, ID3D11Buffer* const*)>
		setCBuffer[static_cast<std::size_t>(EShaderStage::MAX)] = {
		&ID3D11DeviceContext1::VSSetConstantBuffers,
		&ID3D11DeviceContext1::GSSetConstantBuffers,
		&ID3D11DeviceContext1::DSSetConstantBuffers,
		&ID3D11DeviceContext1::HSSetConstantBuffers,
		&ID3D11DeviceContext1::PSSetConstantBuffers,
		&ID3D11DeviceContext1::CSSetConstantBuffers, };

}


/// @brief コンストラクタ
D3D11RendererManager::D3D11RendererManager() :
	m_backBufferFormat(DXGI_FORMAT_R8G8B8A8_UNORM), 
	m_depthStencilFormat(DXGI_FORMAT_D32_FLOAT),
	m_backBufferCount(2),
	m_nOutputWidth(1), 
	m_nOutputHeight(1),
	m_dxgiMSAA(DXGI_SAMPLE_DESC{1, 0}),
	m_vireport(),
	m_hWnd(nullptr),
	m_bUseMSAA(false),
	m_curD3DShader(nullptr),
	m_curBlendState(EBlendState::NONE),
	m_curRasterizeState(ERasterizeState::CULL_BACK),
	m_curDepthStencilState(EDepthStencilState::ENABLE_TEST_AND_ENABLE_WRITE)
{
}

/// @brief コンストラクタ
D3D11RendererManager::~D3D11RendererManager()
{
}

/// @brief 初期化
/// @param hWnd ウィンドウハンドル
/// @param width ウィンドウの幅
/// @param height ウィンドウの高さ
/// @return 成功 TRUE 失敗 FALSE
HRESULT D3D11RendererManager::initialize(HWND hWnd, int width, int height)
{
	// 初期化
	m_hWnd = hWnd;
	m_nOutputWidth = width;
	m_nOutputHeight = height;

	// デバイスの初期化
	CHECK_FAILED(createDivece());

	// スワップチェーンとバッファの初期化
	CHECK_FAILED(createSwapChainAndBuffer());

	// 共通ステートの初期化
	CHECK_FAILED(createCommonState());

	// 共通CBufferの初期化
	D3D11_BUFFER_DESC d3dDesc = {};
	d3dDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	d3dDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dDesc.CPUAccessFlags = 0;
	d3dDesc.MiscFlags = 0;

	// System
	d3dDesc.ByteWidth = sizeof(D3D::SystemBuffer);
	CHECK_FAILED(m_d3dDevice->CreateBuffer(&d3dDesc, nullptr, m_systemBuffer.ReleaseAndGetAddressOf()));
	// Transform
	d3dDesc.ByteWidth = sizeof(D3D::TransformBuffer);
	CHECK_FAILED(m_d3dDevice->CreateBuffer(&d3dDesc, nullptr, m_transformBuffer.ReleaseAndGetAddressOf()));
	// Material
	d3dDesc.ByteWidth = sizeof(D3D::MaterialBuffer);
	CHECK_FAILED(m_d3dDevice->CreateBuffer(&d3dDesc, nullptr, m_materialBuffer.ReleaseAndGetAddressOf()));


	//--- imgui

	
	return S_OK;
}

/// @brief 終了処理
void D3D11RendererManager::finalize()
{
	//--- imgui


}

/// @brief バッファクリア
void D3D11RendererManager::clear()
{
	//--- imgui


	// バッファのクリア
	m_d3dAnnotation->BeginEvent(L"Clear");

	m_d3dContext->OMSetBlendState(nullptr, nullptr, std::numeric_limits<std::uint32_t>::max());

	float ClearColor[4] = { 0.117647f, 0.254902f, 0.352941f, 1.0f };
	m_d3dContext->ClearRenderTargetView(m_backBufferRTV.Get(), ClearColor);
	m_d3dContext->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	m_d3dContext->OMSetRenderTargets(1, m_backBufferRTV.GetAddressOf(), m_depthStencilView.Get());

	m_d3dContext->RSSetViewports(1, &m_vireport);

	m_d3dAnnotation->EndEvent();
}

/// @brief スワップ
void D3D11RendererManager::present()
{
	//--- imgui


	//m_swapChain->Present(0, DXGI_PRESENT_ALLOW_TEARING); // ティアリング許容描画
	//m_swapChain->Present(0, 0);		// 非垂直同期描画
	m_swapChain->Present(1, 0);	// 垂直同期描画
}

/// @brief デバイスの生成
/// @return HRESULT
HRESULT D3D11RendererManager::createDivece()
{
	HRESULT hr;
	HWND hWnd = m_hWnd;

	D3D_FEATURE_LEVEL featureLevels[] = {
	D3D_FEATURE_LEVEL_11_1,
	D3D_FEATURE_LEVEL_11_0,
	D3D_FEATURE_LEVEL_10_1,
	D3D_FEATURE_LEVEL_10_0,
	D3D_FEATURE_LEVEL_9_3,
	D3D_FEATURE_LEVEL_9_2,
	D3D_FEATURE_LEVEL_9_1,
	};

	// ファクトリーの取得
	hr = CreateDXGIFactory1(__uuidof(IDXGIFactory), (void**)(&m_dxgiFactory));
	if (FAILED(hr)) {
		MessageBoxW(hWnd, L"CreateDXGIFactory1", L"Err", MB_ICONSTOP);
		return hr;
	}
	// アダプターの取得
	IDXGIAdapter* iAdapter = NULL;
	std::vector<IDXGIAdapter*> aiAdapter;
	aiAdapter.push_back(iAdapter);
	for (unsigned int index = 0;; index++)
	{
		HRESULT	ret = m_dxgiFactory->EnumAdapters(index, &iAdapter);

		if (ret == DXGI_ERROR_NOT_FOUND)
		{
			break;
		}
		// ～ アダプタの選択
		aiAdapter.push_back(iAdapter);
	}
	aiAdapter.pop_back();
	if (aiAdapter.size() <= 0)
	{
		aiAdapter.push_back(NULL);
	}


	//--- デバイスの生成 ---
	ComPtr<ID3D11Device>		device;
	ComPtr<ID3D11DeviceContext> context;
	ComPtr<ID3D11DeviceContext> defferedContext;

	// デバイス。コンテキストの生成
	hr = D3D11CreateDevice(
		aiAdapter.back(),
		aiAdapter.back() ? D3D_DRIVER_TYPE_UNKNOWN : D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		0,
		featureLevels,
		7,
		D3D11_SDK_VERSION,
		device.GetAddressOf(),
		NULL,
		context.GetAddressOf());
	if (FAILED(hr)) {
		MessageBoxW(hWnd, L"D3D11CreateDevice", L"Err", MB_ICONSTOP);
		return hr;
	}

	// 遅延コンテキスト作成
	hr = device->CreateDeferredContext(0, defferedContext.GetAddressOf());
	if (FAILED(hr)) {
		MessageBoxW(hWnd, L"CreateDeferredContext", L"Err", MB_ICONSTOP);
		return hr;
	}

	// 格納
	hr = device.As(&m_d3dDevice);
	hr = context.As(&m_d3dContext);
	hr = context.As(&m_d3dAnnotation);
	hr = defferedContext.As(&m_d3dDefferedContext);
	if (FAILED(hr)) {
		MessageBoxW(hWnd, L"device,context,defferedContext", L"Err", MB_ICONSTOP);
		return hr;
	}

	//使用可能なMSAAを取得
	if (m_bUseMSAA)
	{
		for (int i = 0; i <= D3D11_MAX_MULTISAMPLE_SAMPLE_COUNT; i++) {
			UINT Quality;
			if SUCCEEDED(device->CheckMultisampleQualityLevels(m_backBufferFormat, i, &Quality)) {
				if (0 < Quality) {
					m_dxgiMSAA.Count = i;
					m_dxgiMSAA.Quality = Quality - 1;
				}
			}
		}
	}


	return S_OK;
}

/// @brief スワップチェーンとバッファの生成
/// @return HRESULT
HRESULT D3D11RendererManager::createSwapChainAndBuffer()
{
	HRESULT hr;
	HWND hWnd = m_hWnd;

	// レンダーターゲットのサイズ指定
	std::uint32_t backBufferWidth =
		std::max<std::uint32_t>(static_cast<std::uint32_t>(m_nOutputWidth), 1);
	std::uint32_t backBufferHeight =
		std::max<std::uint32_t>(static_cast<std::uint32_t>(m_nOutputHeight), 1);

	//--- スワップチェイン ---
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.Width = backBufferWidth;
	swapChainDesc.Height = backBufferHeight;
	swapChainDesc.Format = m_backBufferFormat;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = m_backBufferCount;
	swapChainDesc.SampleDesc = m_dxgiMSAA;
	swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
	swapChainDesc.Flags = 0;
	//swapChainDesc.Flags = (m_options & ALLOW_TEARING) ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;
	// フルスクリーン指定
	DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsSwapChainDesc = {};
	fsSwapChainDesc.Windowed = TRUE;

	// スワップチェーンの作成
	hr = m_dxgiFactory->CreateSwapChainForHwnd(
		m_d3dDevice.Get(),
		m_hWnd,
		&swapChainDesc,
		&fsSwapChainDesc,
		nullptr,
		m_swapChain.ReleaseAndGetAddressOf());
	if (FAILED(hr)) {
		MessageBoxW(hWnd, L"CreateSwapChainForHwnd", L"Err", MB_ICONSTOP);
		return hr;
	}

	// 排他的フルスクリーンモードを無効化
	hr = m_dxgiFactory->MakeWindowAssociation(m_hWnd, DXGI_MWA_NO_ALT_ENTER);
	if (FAILED(hr)) {
		MessageBoxW(hWnd, L"MakeWindowAssociation", L"Err", MB_ICONSTOP);
		return hr;
	}


	//--- バックバッファRT ---

	// フォーマットサポートチェック
	std::uint32_t formatSupport = 0;
	if (FAILED(m_d3dDevice->CheckFormatSupport(m_backBufferFormat, &formatSupport)))
	{
		MessageBoxW(hWnd, L"CheckFormatSupport", L"Err", MB_ICONSTOP);
		return E_FAIL;
	}

	// バックバッファレンダーターゲットの作成
	m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
		(LPVOID*)m_backBufferRT.ReleaseAndGetAddressOf());

	CD3D11_RENDER_TARGET_VIEW_DESC rtvDesc(D3D11_RTV_DIMENSION_TEXTURE2D, m_backBufferFormat);
	if (m_bUseMSAA) rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
	// レンダーターゲットビュー作成
	hr = m_d3dDevice->CreateRenderTargetView(
		m_backBufferRT.Get(),
		&rtvDesc,
		m_backBufferRTV.ReleaseAndGetAddressOf());
	if (FAILED(hr)) {
		MessageBoxW(hWnd, L"CreateRenderTargetView", L"Err", MB_ICONSTOP);
		return hr;
	}


	//--- 深度ステンシル ---
	CD3D11_TEXTURE2D_DESC dsDesc(m_depthStencilFormat, backBufferWidth, backBufferHeight,
		1, 1, D3D11_BIND_DEPTH_STENCIL);
	if (m_bUseMSAA) dsDesc.SampleDesc = m_dxgiMSAA;
	// 深度ステンシルテクスチャ作成
	hr = m_d3dDevice->CreateTexture2D(&dsDesc, nullptr,
		m_depthStencilTexture.ReleaseAndGetAddressOf());
	CHECK_FAILED(hr);
	// 深度ステンシルテクスチャビュー
	CD3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc(D3D11_DSV_DIMENSION_TEXTURE2D);
	if (m_bUseMSAA) dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
	hr = m_d3dDevice->CreateDepthStencilView(m_depthStencilTexture.Get(),
		&dsvDesc, m_depthStencilView.ReleaseAndGetAddressOf());
	CHECK_FAILED(hr);


	// ビューポート
	m_vireport = CD3D11_VIEWPORT(0.0f, 0.0f,
		static_cast<float>(backBufferWidth),
		static_cast<float>(backBufferHeight));

	return S_OK;
}

/// @brief 共通ステートの生成
/// @return HRESULT
HRESULT D3D11RendererManager::createCommonState()
{
	// ラスタライザステート作成
	{
		D3D11_RASTERIZER_DESC rasterizerDesc = {};

		// カリングなし 塗りつぶし
		rasterizerDesc.FillMode = D3D11_FILL_SOLID;
		rasterizerDesc.CullMode = D3D11_CULL_NONE;
		CHECK_FAILED(m_d3dDevice->CreateRasterizerState(&rasterizerDesc,
			m_rasterizeStates[(size_t)ERasterizeState::CULL_NONE].GetAddressOf()));

		// 表面カリング 塗りつぶし
		rasterizerDesc.CullMode = D3D11_CULL_FRONT;
		CHECK_FAILED(m_d3dDevice->CreateRasterizerState(&rasterizerDesc,
			m_rasterizeStates[(size_t)ERasterizeState::CULL_FRONT].GetAddressOf()));

		// 裏面カリング 塗りつぶし
		rasterizerDesc.CullMode = D3D11_CULL_BACK;
		CHECK_FAILED(m_d3dDevice->CreateRasterizerState(&rasterizerDesc,
			m_rasterizeStates[(size_t)ERasterizeState::CULL_BACK].GetAddressOf()));

		// カリングなし ワイヤーフレーム
		rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
		rasterizerDesc.CullMode = D3D11_CULL_NONE;
		CHECK_FAILED(m_d3dDevice->CreateRasterizerState(&rasterizerDesc,
			m_rasterizeStates[(size_t)ERasterizeState::CULL_NONE_WIREFRAME].GetAddressOf()));

		// 表面カリング ワイヤーフレーム
		rasterizerDesc.CullMode = D3D11_CULL_FRONT;
		CHECK_FAILED(m_d3dDevice->CreateRasterizerState(&rasterizerDesc,
			m_rasterizeStates[(size_t)ERasterizeState::CULL_FRONT_WIREFRAME].GetAddressOf()));

		// 裏面カリング ワイヤーフレーム
		rasterizerDesc.CullMode = D3D11_CULL_BACK;
		CHECK_FAILED(m_d3dDevice->CreateRasterizerState(&rasterizerDesc,
			m_rasterizeStates[(size_t)ERasterizeState::CULL_BACK_WIREFRAME].GetAddressOf()));

		// シャドウ用
		rasterizerDesc.FillMode					= D3D11_FILL_SOLID;
		rasterizerDesc.CullMode					= D3D11_CULL_BACK;
		rasterizerDesc.FrontCounterClockwise	= false;
		rasterizerDesc.DepthClipEnable			= true;
		rasterizerDesc.DepthBias				= 100000;
		rasterizerDesc.DepthBiasClamp			= 0.0f;
		rasterizerDesc.SlopeScaledDepthBias		= 2.0f;
		CHECK_FAILED(m_d3dDevice->CreateRasterizerState(&rasterizerDesc,
			m_rasterizeStates[(size_t)ERasterizeState::SHADOW].GetAddressOf()));
	}

	// サンプラステート作成
	{
		D3D11_SAMPLER_DESC samplerDesc = {};

		samplerDesc.MipLODBias = 0.0f;
		samplerDesc.MaxAnisotropy = 1;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		samplerDesc.BorderColor[0] = 0.f;
		samplerDesc.BorderColor[1] = 0.f;
		samplerDesc.BorderColor[2] = 0.f;
		samplerDesc.BorderColor[3] = 0.f;
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
		samplerDesc.MaxAnisotropy = (m_d3dDevice->GetFeatureLevel() > D3D_FEATURE_LEVEL_9_1) ? D3D11_MAX_MAXANISOTROPY : 2;

		// リニアクランプ
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		CHECK_FAILED(m_d3dDevice->CreateSamplerState(&samplerDesc, m_samplerStates[static_cast<size_t>(ESamplerState::LINEAR_CLAMP)].GetAddressOf()));

		// ポイントクランプ
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		CHECK_FAILED(m_d3dDevice->CreateSamplerState(&samplerDesc, m_samplerStates[static_cast<size_t>(ESamplerState::POINT_CLAMP)].GetAddressOf()));

		// 異方性クランプ
		samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
		CHECK_FAILED(m_d3dDevice->CreateSamplerState(&samplerDesc, m_samplerStates[static_cast<size_t>(ESamplerState::ANISOTROPIC_CLAMP)].GetAddressOf()));

		// リニアラップ
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		CHECK_FAILED(m_d3dDevice->CreateSamplerState(&samplerDesc, m_samplerStates[static_cast<size_t>(ESamplerState::LINEAR_WRAP)].GetAddressOf()));

		// ポイントラップ
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		CHECK_FAILED(m_d3dDevice->CreateSamplerState(&samplerDesc, m_samplerStates[static_cast<size_t>(ESamplerState::POINT_WRAP)].GetAddressOf()));

		// 異方性ラップ
		samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
		CHECK_FAILED(m_d3dDevice->CreateSamplerState(&samplerDesc, m_samplerStates[static_cast<size_t>(ESamplerState::ANISOTROPIC_WRAP)].GetAddressOf()));

		// シャドウ
		samplerDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
		samplerDesc.BorderColor[0] = 1.0f;
		samplerDesc.BorderColor[1] = 1.0f;
		samplerDesc.BorderColor[2] = 1.0f;
		samplerDesc.BorderColor[3] = 1.0f;
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
		CHECK_FAILED(m_d3dDevice->CreateSamplerState(&samplerDesc, m_samplerStates[static_cast<size_t>(ESamplerState::SHADOW)].GetAddressOf()));
	}

	// ブレンドステート作成
	{
		D3D11_BLEND_DESC blendDesc = {};

		blendDesc.AlphaToCoverageEnable = false;
		blendDesc.IndependentBlendEnable = false;
		blendDesc.RenderTarget[0].BlendEnable = true;
		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		// 半透明合成
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		CHECK_FAILED(m_d3dDevice->CreateBlendState(&blendDesc, m_blendStates[static_cast<size_t>(EBlendState::ALPHA)].GetAddressOf()));

		// 加算合成
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
		CHECK_FAILED(m_d3dDevice->CreateBlendState(&blendDesc, m_blendStates[static_cast<size_t>(EBlendState::ADD)].GetAddressOf()));

		// 減算合成
		blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_REV_SUBTRACT;
		CHECK_FAILED(m_d3dDevice->CreateBlendState(&blendDesc, m_blendStates[static_cast<size_t>(EBlendState::SUB)].GetAddressOf()));

		// 乗算合成
		blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ZERO;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_SRC_COLOR;
		CHECK_FAILED(m_d3dDevice->CreateBlendState(&blendDesc, m_blendStates[static_cast<size_t>(EBlendState::MUL)].GetAddressOf()));

		// 反転合成
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_INV_DEST_COLOR;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
		CHECK_FAILED(m_d3dDevice->CreateBlendState(&blendDesc, m_blendStates[static_cast<size_t>(EBlendState::INV)].GetAddressOf()));
	}

	// 深度ステンシルステート作成
	{
		m_depthStencilStates[static_cast<size_t>(EDepthStencilState::UNKNOWN)] = nullptr;

		D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};

		depthStencilDesc.DepthEnable = true;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;

		CHECK_FAILED(m_d3dDevice->CreateDepthStencilState(&depthStencilDesc, m_depthStencilStates[static_cast<size_t>(EDepthStencilState::ENABLE_TEST_AND_ENABLE_WRITE)].GetAddressOf()));

		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;

		CHECK_FAILED(m_d3dDevice->CreateDepthStencilState(&depthStencilDesc, m_depthStencilStates[static_cast<size_t>(EDepthStencilState::ENABLE_TEST_AND_DISABLE_WRITE)].GetAddressOf()));

		depthStencilDesc.DepthEnable = false;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_NEVER;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;

		CHECK_FAILED(m_d3dDevice->CreateDepthStencilState(&depthStencilDesc, m_depthStencilStates[static_cast<size_t>(EDepthStencilState::DISABLE_TEST_AND_ENABLE_WRITE)].GetAddressOf()));

		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;

		CHECK_FAILED(m_d3dDevice->CreateDepthStencilState(&depthStencilDesc, m_depthStencilStates[static_cast<size_t>(EDepthStencilState::DISABLE_TEST_AND_DISABLE_WRITE)].GetAddressOf()));
	}

	return S_OK;
}

/// @brief D3D11マテリアルの指定
/// @param materialID マテリアルID
void D3D11RendererManager::setD3D11Material(const MaterialID& materialID)
{
	// マテリアルの取得
	auto* d3dMat = static_cast<D3D11Material*>(getMaterial(materialID));
	if (d3dMat == nullptr) return;

	// シェーダーの取得
	auto* d3dShader = static_cast<D3D11Shader*>(getShader(d3dMat->m_shaderID));
	if (d3dShader == nullptr) return;

	// シェーダーの指定
	if (m_curD3DShader != d3dShader)
	{
		if (d3dShader->vs) m_d3dContext->VSSetShader(d3dShader->vs, nullptr, 0);
		if (d3dShader->gs) m_d3dContext->GSSetShader(d3dShader->gs, nullptr, 0);
		if (d3dShader->ds) m_d3dContext->DSSetShader(d3dShader->ds, nullptr, 0);
		if (d3dShader->hs) m_d3dContext->HSSetShader(d3dShader->hs, nullptr, 0);
		if (d3dShader->ps) m_d3dContext->PSSetShader(d3dShader->ps, nullptr, 0);
		if (d3dShader->cs) m_d3dContext->CSSetShader(d3dShader->cs, nullptr, 0);
		// 入力レイアウト指定
		m_d3dContext->IASetInputLayout(d3dShader->m_inputLayout.Get());
		m_curD3DShader = d3dShader;
	}

	// ブレンドステート
	if (m_curBlendState != d3dMat->m_blendState)
	{
		constexpr float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
		m_d3dContext->OMSetBlendState(m_blendStates[
			static_cast<std::size_t>(d3dMat->m_blendState)].Get(), blendFactor, 0xffffffff);
		m_curBlendState = d3dMat->m_blendState;
	}
	// ラスタライザステート
	if (m_curRasterizeState != d3dMat->m_rasterizeState)
	{
		m_d3dContext->RSSetState(m_rasterizeStates[
			static_cast<std::size_t>(d3dMat->m_rasterizeState)].Get());
		m_curRasterizeState = d3dMat->m_rasterizeState;
	}
	// 深度ステンシルステート
	if (m_curDepthStencilState != d3dMat->m_depthStencilState)
	{
		if (d3dMat->m_isTransparent)
		{
			m_d3dContext->OMSetDepthStencilState(m_depthStencilStates[
				static_cast<std::size_t>(EDepthStencilState::ENABLE_TEST_AND_DISABLE_WRITE)].Get(), 0);
			m_curDepthStencilState = EDepthStencilState::ENABLE_TEST_AND_DISABLE_WRITE;
		}
		else
		{
			m_d3dContext->OMSetDepthStencilState(m_depthStencilStates[
				static_cast<std::size_t>(d3dMat->m_depthStencilState)].Get(), 0);
			m_curDepthStencilState = d3dMat->m_depthStencilState;
		}
	}

	// マテリアルリソース指定・更新
	setD3D11MaterialResource(*d3dMat, *d3dShader);

}

/// @brief D3D11マテリアルのリソースを指定
/// @param d3dMaterial D3D11マテリアルの参照
void D3D11RendererManager::setD3D11MaterialResource(const D3D11Material& d3dMaterial, const D3D11Shader& d3dShader)
{
	auto& d3dMat = const_cast<D3D11Material&>(d3dMaterial);

	// ステージごと
	for (auto stage = EShaderStage::VS; stage < EShaderStage::MAX; ++stage)
	{
		if (!hasStaderStage(d3dShader.m_desc.m_stages, stage)) continue;

		auto stageIndex = static_cast<std::size_t>(stage);

		// マテリアルバッファ更新
		setCBuffer[stageIndex](m_d3dContext.Get(), D3D::SHADER_CB_SLOT_MATERIAL, 1, m_materialBuffer.GetAddressOf());
		m_d3dContext->UpdateSubresource(m_materialBuffer.Get(), 0, nullptr, &d3dMat.m_materialBuffer, 0, 0);

		// コンスタントバッファ
		for (const auto& cbuffer : d3dMaterial.m_d3dCbuffer[stageIndex]) 
		{
			// 指定
			setCBuffer[stageIndex](m_d3dContext.Get(), cbuffer.first, 1, cbuffer.second.GetAddressOf());
			// 更新
			if (d3dMat.m_cbufferData[stageIndex][cbuffer.first].isUpdate)
			{
				m_d3dContext->UpdateSubresource(cbuffer.second.Get(), 0, nullptr,
					d3dMat.m_cbufferData[stageIndex][cbuffer.first].data.get(), 0, 0);
				d3dMat.m_cbufferData[stageIndex][cbuffer.first].isUpdate = false;
			}
		}

		// テクスチャ更新


		// サンプラ更新

	}

}

void D3D11RendererManager::setD3DSystemBuffer(const D3D::SystemBuffer& systemBuffer)
{
	for (auto stage = EShaderStage::VS; stage < EShaderStage::MAX; ++stage)
	{
		auto stageIndex = static_cast<std::size_t>(stage);
		setCBuffer[stageIndex](m_d3dContext.Get(), D3D::SHADER_CB_SLOT_SYSTEM, 1, m_systemBuffer.GetAddressOf());
		m_d3dContext->UpdateSubresource(m_systemBuffer.Get(), 0, nullptr, &systemBuffer, 0, 0);
	}
}

void D3D11RendererManager::setD3DTransformBuffer(const Matrix& mtxWorld)
{
	D3D::TransformBuffer transform;
	transform._mWorld = mtxWorld.Transpose();

	for (auto stage = EShaderStage::VS; stage < EShaderStage::MAX; ++stage)
	{
		auto stageIndex = static_cast<std::size_t>(stage);
		setCBuffer[stageIndex](m_d3dContext.Get(), D3D::SHADER_CB_SLOT_TRANSFORM, 1, m_transformBuffer.GetAddressOf());
		m_d3dContext->UpdateSubresource(m_transformBuffer.Get(), 0, nullptr, &transform, 0, 0);
	}
}

void D3D11RendererManager::render(const MaterialID& materialID, const MeshID& meshID)
{
	auto* context = m_d3dContext.Get();

	// データの取得
	auto* material = static_cast<D3D11Material*>(getMaterial(materialID));
	auto* shader = static_cast<D3D11Shader*>(getShader(material->m_shaderID));
	const auto& rdID = createRenderBuffer(shader->m_id, meshID);
	auto* renderBuffer = static_cast<D3D11RenderBuffer*>(getRenderBuffer(rdID));

	// 頂点バッファをセット
	UINT stride = static_cast<UINT>(renderBuffer->m_vertexData.size);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, renderBuffer->m_vertexBuffer.GetAddressOf(), &stride, &offset);
	// インデックスバッファをセット
	if (renderBuffer->m_indexData.count > 0) {
		context->IASetIndexBuffer(renderBuffer->m_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	}

	// プリミティブ指定
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// ポリゴンの描画
	if (renderBuffer->m_indexData.count > 0)
	{
		context->DrawIndexed(renderBuffer->m_indexData.count, 0, 0);
	}
	else
	{
		context->Draw(renderBuffer->m_vertexData.count, 0);
	}
}


ShaderID D3D11RendererManager::createShader(ShaderDesc desc)
{
	// IDの取得
	ShaderID id = hash::crc32string(desc.m_name.c_str());

	// 既に生成済み
	const auto& itr = m_shaderPool.find(id);
	if (m_shaderPool.end() != itr) return id;

	// 新規生成
	m_shaderPool[id] = std::make_unique<D3D11Shader>(m_d3dDevice.Get(), desc, id);

	return id;
}

MaterialID D3D11RendererManager::createMaterial(std::string name, ShaderID shaderID)
{
	// IDの取得
	MaterialID id = hash::crc32string(name.c_str());

	// 既に生成済み
	const auto& itr = m_materialPool.find(id);
	if (m_materialPool.end() != itr) return id;

	// シェーダー取得
	auto* shader = getShader(shaderID);
	if (shader == nullptr) return NONE_MATERIAL_ID;

	// 新規生成
	m_materialPool[id] = std::make_unique<D3D11Material>(
		m_d3dDevice.Get(), id, name, *shader);

	return id;
}

MeshID D3D11RendererManager::createMesh(std::string name)
{
	// IDの取得
	MeshID id = hash::crc32string(name.c_str());

	// 既に生成済み
	const auto& itr = m_meshPool.find(id);
	if (m_meshPool.end() != itr) return id;

	// 新規生成
	m_meshPool[id] = std::make_unique<Mesh>(id, name);

	return id;
}

RenderBufferID D3D11RendererManager::createRenderBuffer(ShaderID shaderID, MeshID meshID)
{
	// IDの生成
	RenderBufferID id;
	std::memcpy(&id, &shaderID, sizeof(std::uint32_t));
	std::memcpy(&id + sizeof(std::uint32_t), &meshID, sizeof(std::uint32_t));
	
	// 既にある
	const auto& itr = m_renderBufferPool.find(id);
	if (m_renderBufferPool.end() != itr) return id;

	// シェーダー取得
	auto* shader = getShader(shaderID);
	if (shader == nullptr) return NONE_RENDERBUFFER_ID;
	// メッシュ取得
	auto* mesh = getMesh(meshID);
	if (mesh == nullptr) return NONE_RENDERBUFFER_ID;

	// 新規生成
	m_renderBufferPool[id] = std::make_unique<D3D11RenderBuffer>(
		m_d3dDevice.Get(), id, *shader, *mesh);

	return id;
}

TextureID D3D11RendererManager::createTexture()
{
	return NONE_TEXTURE_ID;
}

