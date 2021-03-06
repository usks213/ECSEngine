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

#include "D3D11Buffer.h"
#include "D3D11Shader.h"
#include "D3D11Material.h"
#include "D3D11RenderBuffer.h"
#include "D3D11Texture.h"
#include "D3D11RenderTarget.h"
#include "D3D11DepthStencil.h"

//#ifdef _DEBUG
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "ImGuizmo.h"
//#include "ImGuiUtils.h"
//#endif

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
	// CBuffer
	static std::function<void(ID3D11DeviceContext1*, UINT, UINT, ID3D11Buffer* const*)>
		setCBuffer[static_cast<std::size_t>(ShaderStage::MAX)] = {
		&ID3D11DeviceContext1::VSSetConstantBuffers,
		&ID3D11DeviceContext1::GSSetConstantBuffers,
		&ID3D11DeviceContext1::DSSetConstantBuffers,
		&ID3D11DeviceContext1::HSSetConstantBuffers,
		&ID3D11DeviceContext1::PSSetConstantBuffers,
		&ID3D11DeviceContext1::CSSetConstantBuffers, };
	// SRV
	static std::function<void(ID3D11DeviceContext1*, UINT, UINT, ID3D11ShaderResourceView* const*)>
		setShaderResource[static_cast<std::size_t>(ShaderStage::MAX)] = {
		&ID3D11DeviceContext1::VSSetShaderResources,
		&ID3D11DeviceContext1::GSSetShaderResources,
		&ID3D11DeviceContext1::DSSetShaderResources,
		&ID3D11DeviceContext1::HSSetShaderResources,
		&ID3D11DeviceContext1::PSSetShaderResources,
		&ID3D11DeviceContext1::CSSetShaderResources, };
	// Sampler
	static std::function<void(ID3D11DeviceContext1*, UINT, UINT, ID3D11SamplerState* const*)>
		setSamplers[static_cast<std::size_t>(ShaderStage::MAX)] = {
		&ID3D11DeviceContext1::VSSetSamplers,
		&ID3D11DeviceContext1::GSSetSamplers,
		&ID3D11DeviceContext1::DSSetSamplers,
		&ID3D11DeviceContext1::HSSetSamplers,
		&ID3D11DeviceContext1::PSSetSamplers,
		&ID3D11DeviceContext1::CSSetSamplers, };
}


/// @brief コンストラクタ
D3D11RendererManager::D3D11RendererManager() :
	m_backBufferFormat(DXGI_FORMAT_R8G8B8A8_UNORM), 
	m_depthStencilFormat(DXGI_FORMAT_D32_FLOAT),
	m_diffuseFormat(DXGI_FORMAT_R8G8B8A8_UNORM),
	m_normalFormat(DXGI_FORMAT_R16G16B16A16_FLOAT),
	m_backBufferCount(2),
	m_nOutputWidth(1), 
	m_nOutputHeight(1),
	m_dxgiMSAA(DXGI_SAMPLE_DESC{1, 0}),
	m_viewport(),
	m_hWnd(nullptr),
	m_bUseMSAA(false),
	m_curD3DShader(nullptr),
	m_curBlendState(BlendState::NONE),
	m_curRasterizeState(RasterizeState::CULL_NONE),
	m_curDepthStencilState(DepthStencilState::DISABLE_TEST_AND_DISABLE_WRITE),
	m_curRTV(nullptr),
	m_curDSV(nullptr)
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
	d3dDesc.ByteWidth = sizeof(SHADER::SystemBuffer);
	CHECK_FAILED(m_d3dDevice->CreateBuffer(&d3dDesc, nullptr, m_systemBuffer.ReleaseAndGetAddressOf()));
	// Transform
	d3dDesc.ByteWidth = sizeof(SHADER::TransformBuffer);
	CHECK_FAILED(m_d3dDevice->CreateBuffer(&d3dDesc, nullptr, m_transformBuffer.ReleaseAndGetAddressOf()));
	// Material
	d3dDesc.ByteWidth = sizeof(SHADER::MaterialBuffer);
	CHECK_FAILED(m_d3dDevice->CreateBuffer(&d3dDesc, nullptr, m_materialBuffer.ReleaseAndGetAddressOf()));

	// Map/Unmap Param
	d3dDesc.Usage = D3D11_USAGE_DYNAMIC;
	d3dDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	// Animation
	d3dDesc.ByteWidth = sizeof(SHADER::AnimationBuffer);
	CHECK_FAILED(m_d3dDevice->CreateBuffer(&d3dDesc, nullptr, m_animationBuffer.ReleaseAndGetAddressOf()));

	// Light
	d3dDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	d3dDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.FirstElement = 0;

	// ポイントライト
	d3dDesc.ByteWidth = sizeof(PointLightData) * SHADER::MAX_POINT_LIGHT_COUNT;
	d3dDesc.StructureByteStride = sizeof(PointLightData);
	CHECK_FAILED(m_d3dDevice->CreateBuffer(&d3dDesc, nullptr, m_pointLightBuffer.ReleaseAndGetAddressOf()));
	srvDesc.Buffer.NumElements = SHADER::MAX_POINT_LIGHT_COUNT;
	CHECK_FAILED(m_d3dDevice->CreateShaderResourceView(m_pointLightBuffer.Get(), nullptr, m_pointLightSRV.ReleaseAndGetAddressOf()));
	// スポットライト
	d3dDesc.ByteWidth = sizeof(SpotLightData) * SHADER::MAX_SPOT_LIGHT_COUNT;
	d3dDesc.StructureByteStride = sizeof(SpotLightData);
	CHECK_FAILED(m_d3dDevice->CreateBuffer(&d3dDesc, nullptr, m_spotLightBuffer.ReleaseAndGetAddressOf()));
	srvDesc.Buffer.NumElements = SHADER::MAX_SPOT_LIGHT_COUNT;
	CHECK_FAILED(m_d3dDevice->CreateShaderResourceView(m_spotLightBuffer.Get(), nullptr, m_spotLightSRV.ReleaseAndGetAddressOf()));

	// Sampler
	for (auto stage = ShaderStage::VS; stage < ShaderStage::MAX; ++stage) 
	{
		setD3D11Sampler(SHADER::SHADER_SS_SLOT_MAIN,	SamplerState::LINEAR_WRAP,		 stage);
		setD3D11Sampler(SHADER::SHADER_SS_SLOT_SHADOW, SamplerState::SHADOW,			 stage);
		setD3D11Sampler(SHADER::SHADER_SS_SLOT_SKYBOX, SamplerState::ANISOTROPIC_WRAP, stage);
	}


	//--- imgui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;	// ウィンドウになる
	// style
	//ImGui::StyleColorsDark();
	ImGui::StyleColorsClassic();

	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 0.0f;
	}
	// setup platform/renderer
	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX11_Init(m_d3dDevice.Get(), m_d3dContext.Get());

	// 日本語フォント
	ImFontConfig config = {};
	config.OversampleH = 1;
	config.OversampleV = 1;
	config.PixelSnapH = 1;
	io.Fonts->AddFontDefault();
	ImFont* pFont = io.Fonts->AddFontFromFileTTF("data/font/msgothic.ttc", 18.0f * 1.0f, &config, io.Fonts->GetGlyphRangesJapanese());
	IM_ASSERT(pFont != nullptr);
	ImFont* currentFont = ImGui::GetFont();
	currentFont = pFont;
	io.FontDefault = pFont;
	io.IniFilename = "data/config.ini";

	return S_OK;
}

/// @brief 終了処理
void D3D11RendererManager::finalize()
{
	//--- imgui
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

}

/// @brief バッファクリア
void D3D11RendererManager::clear()
{
	//--- imgui
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	ImGuizmo::BeginFrame();

	// debug表示
	imguiDebug();

	// バッファのクリア
	m_d3dAnnotation->BeginEvent(L"Clear");

	m_d3dContext->OMSetBlendState(nullptr, nullptr, std::numeric_limits<std::uint32_t>::max());

	float ClearColor[4] = { 0.2f, 0.22f, 0.22f, 1.0f };
	m_d3dContext->ClearRenderTargetView(m_backBufferRTV.Get(), ClearColor);
	m_d3dContext->ClearRenderTargetView(m_gbuffer.m_diffuseRTV.Get(), ClearColor);
	m_d3dContext->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	//m_d3dContext->OMSetRenderTargets(1, m_backBufferRTV.GetAddressOf(), m_depthStencilView.Get());
	m_d3dContext->OMSetRenderTargets(1, m_gbuffer.m_diffuseRTV.GetAddressOf(), m_depthStencilView.Get());

	m_d3dContext->RSSetViewports(1, &m_viewport);
	m_d3dAnnotation->EndEvent();
}

/// @brief スワップ
void D3D11RendererManager::present()
{
	// バックバッファに戻す
	m_d3dContext->OMSetRenderTargets(1, m_backBufferRTV.GetAddressOf(), m_depthStencilView.Get());

	//--- imgui
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}

	//m_swapChain->Present(0, DXGI_PRESENT_ALLOW_TEARING); // ティアリング許容描画
	//m_swapChain->Present(0, 0);		// 非垂直同期描画
	m_swapChain->Present(1, 0);	// 垂直同期描画
}

/// @brief Imgui表示
void D3D11RendererManager::imguiDebug()
{
	// 全体ウィンドウ
	RECT rcWin, rcClnt;
	GetClientRect(m_hWnd, &rcClnt);
	GetWindowRect(m_hWnd, &rcWin);
	int titleSizeX = (rcWin.right - rcWin.left) - (rcClnt.right - rcClnt.left);
	int titleSizeY = (rcWin.bottom - rcWin.top) - (rcClnt.bottom - rcClnt.top);

	//ImGui::SetNextWindowBgAlpha(0.0f);
	//ImGui::SetNextWindowPos(ImVec2(rcWin.left + titleSizeX / 2, rcWin.top + titleSizeX * 2), ImGuiCond_Always);
	//ImGui::SetNextWindowSize(ImVec2(m_pEngine->getWindowWidth() - 1, m_pEngine->getWindowHeight() - 1), ImGuiCond_Always);
	//if (!ImGui::Begin("Fixed Overlay",0,
	//	ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
	//	ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground))
	//{
	//	ImGui::End();
	//	return;
	//}
	//// メニューバー
	//if (ImGui::BeginMainMenuBar())
	//{
	//	if (ImGui::BeginMenu("File"))
	//	{

	//		ImGui::EndMenu();
	//	}

	//	ImGui::EndMainMenuBar();
	//}
	//ImGui::End();


	// マテリアル
	ImGui::SetNextWindowBgAlpha(0.4f);
	ImGui::Begin("Material");
	for (const auto& mat : m_materialPool)
	{
		if (ImGui::TreeNode(mat.second->m_name.c_str())) {
			auto* d3dMat = static_cast<D3D11Material*>(mat.second.get());
			//--- ID
			ImGui::Text("MaterialID:%u", mat.second->m_id);
			//--- Flg
			{
				// Tex
				bool temp = d3dMat->m_materialBuffer._Flg & (UINT)SHADER::Material_Flg::TEXTURE;
				if (ImGui::Checkbox("Texture", &temp))
					d3dMat->m_materialBuffer._Flg ^= (UINT)SHADER::Material_Flg::TEXTURE;
				// Light
				temp = d3dMat->m_materialBuffer._Flg & (UINT)SHADER::Material_Flg::LIGHT;
				if (ImGui::Checkbox("Lighting", &temp))
					d3dMat->m_materialBuffer._Flg ^= (UINT)SHADER::Material_Flg::LIGHT;
				// Shadow
				temp = d3dMat->m_materialBuffer._Flg & (UINT)SHADER::Material_Flg::SHADOW;
				if (ImGui::Checkbox("Shadow", &temp))
					d3dMat->m_materialBuffer._Flg ^= (UINT)SHADER::Material_Flg::SHADOW;
				// Fog
				temp = d3dMat->m_materialBuffer._Flg & (UINT)SHADER::Material_Flg::FOG;
				if (ImGui::Checkbox("Fog", &temp))
					d3dMat->m_materialBuffer._Flg ^= (UINT)SHADER::Material_Flg::FOG;
			}
			//Color
			ImGui::SliderFloat4("Color", (float*)&d3dMat->m_materialBuffer._Color, 0.0f, 1.0f);
			//ImGui::InputText("textbox 1", text1, sizeof(text1));

			// CBuffer変数
			for (const auto& var : mat.second->m_cbufferVariable)
			{
				float temp = *(float*)mat.second->getData(var.second.name.c_str());
				if (ImGui::InputFloat(var.second.name.c_str(), &temp))
					mat.second->setFloat(var.second.name.c_str(), temp);
			}

			for(auto stage = ShaderStage::VS; stage < ShaderStage::MAX; ++stage)
			{
				auto stageIndex = static_cast<size_t>(stage);
				// テクスチャ
				for (auto& tex : d3dMat->m_textureData[stageIndex])
				{
					static bool texWind;
					static std::string texName;

					auto* pTex = static_cast<D3D11Texture*>(getTexture(tex.second.id));
					ID3D11ShaderResourceView* pSRV = nullptr;
					if (pTex) pSRV = pTex->m_srv.Get();

					ImGui::Text(tex.second.name.c_str());
					if (ImGui::ImageButton(pSRV, ImVec2(100, 100)))
					{
						texWind ^= 1;
						texName = d3dMat->m_name + ":" + tex.second.name;
					}

					if (texWind && texName == std::string(d3dMat->m_name + ":" + tex.second.name))
					{
						//ImGui::SetNextWindowSize(ImVec2(150, 400));
						ImGui::SetNextWindowBgAlpha(0.8f);
						ImGui::Begin(texName.c_str());

						if (ImGui::Button("NONE", ImVec2(100, 100)))
						{
							texWind ^= 1;
							tex.second.id = NONE_TEXTURE_ID;
						}

						for (const auto& tex2 : m_texturePool)
						{
							ImGui::Text(tex2.second->m_name.c_str());
							auto* pTex = static_cast<D3D11Texture*>(tex2.second.get());
							if (ImGui::ImageButton(pTex->m_srv.Get(), ImVec2(100, 100)))
							{
								texWind ^= 1;
								tex.second.id = tex2.first;
							}
						}
						ImGui::End();
					}
				}

				// サンプラー


			}

			ImGui::TreePop();
		}
	}

	ImGui::End();
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
		// 〜 アダプタの選択
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

	//----- GBUFFER -----
	//--- 拡散反射光バッファ
	// テクスチャ2D
	D3D11_TEXTURE2D_DESC renderTexture;
	m_backBufferRT->GetDesc(&renderTexture);
	renderTexture.Format = m_diffuseFormat;
	renderTexture.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
	hr = m_d3dDevice->CreateTexture2D(
		&renderTexture,
		nullptr,
		m_gbuffer.m_diffuseRT.ReleaseAndGetAddressOf()
	);
	CHECK_FAILED(hr);
	// レンダラーターゲットビュー
	rtvDesc.Format = m_diffuseFormat;
	hr = m_d3dDevice->CreateRenderTargetView(
		m_gbuffer.m_diffuseRT.Get(),
		&rtvDesc,
		m_gbuffer.m_diffuseRTV.ReleaseAndGetAddressOf());
	CHECK_FAILED(hr);
	// シェーダーリソース
	CD3D11_SHADER_RESOURCE_VIEW_DESC srvDesc(D3D11_SRV_DIMENSION_TEXTURE2D, m_diffuseFormat);
	if (m_bUseMSAA) srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
	hr = m_d3dDevice->CreateShaderResourceView(
		m_gbuffer.m_diffuseRT.Get(),
		&srvDesc,
		m_gbuffer.m_diffuseSRV.ReleaseAndGetAddressOf()
	);
	CHECK_FAILED(hr);

	//--- ワールド法線バッファ
	// テクスチャ2D
	renderTexture.Format = m_normalFormat;
	hr = m_d3dDevice->CreateTexture2D(
		&renderTexture,
		nullptr,
		m_gbuffer.m_normalRT.ReleaseAndGetAddressOf()
	);
	CHECK_FAILED(hr);
	// レンダラーターゲットビュー
	rtvDesc.Format = m_normalFormat;
	hr = m_d3dDevice->CreateRenderTargetView(
		m_gbuffer.m_normalRT.Get(),
		&rtvDesc,
		m_gbuffer.m_normalRTV.ReleaseAndGetAddressOf());
	CHECK_FAILED(hr);
	// シェーダーリソース
	srvDesc.Format = m_normalFormat;
	hr = m_d3dDevice->CreateShaderResourceView(
		m_gbuffer.m_normalRT.Get(),
		&srvDesc,
		m_gbuffer.m_normalSRV.ReleaseAndGetAddressOf()
	);
	CHECK_FAILED(hr);

	//--- ワールド座標バッファ
	// テクスチャ2D
	hr = m_d3dDevice->CreateTexture2D(
		&renderTexture,
		nullptr,
		m_gbuffer.m_positionRT.ReleaseAndGetAddressOf()
	);
	CHECK_FAILED(hr);
	// レンダラーターゲットビュー
	hr = m_d3dDevice->CreateRenderTargetView(
		m_gbuffer.m_positionRT.Get(),
		&rtvDesc,
		m_gbuffer.m_positionRTV.ReleaseAndGetAddressOf());
	CHECK_FAILED(hr);
	// シェーダーリソース
	hr = m_d3dDevice->CreateShaderResourceView(
		m_gbuffer.m_positionRT.Get(),
		&srvDesc,
		m_gbuffer.m_positionSRV.ReleaseAndGetAddressOf()
	);
	CHECK_FAILED(hr);

	// ビューポート
	m_viewport = CD3D11_VIEWPORT(0.0f, 0.0f,
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
			m_rasterizeStates[(size_t)RasterizeState::CULL_NONE].GetAddressOf()));

		// 表面カリング 塗りつぶし
		rasterizerDesc.CullMode = D3D11_CULL_FRONT;
		CHECK_FAILED(m_d3dDevice->CreateRasterizerState(&rasterizerDesc,
			m_rasterizeStates[(size_t)RasterizeState::CULL_FRONT].GetAddressOf()));

		// 裏面カリング 塗りつぶし
		rasterizerDesc.CullMode = D3D11_CULL_BACK;
		CHECK_FAILED(m_d3dDevice->CreateRasterizerState(&rasterizerDesc,
			m_rasterizeStates[(size_t)RasterizeState::CULL_BACK].GetAddressOf()));

		// カリングなし ワイヤーフレーム
		rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
		rasterizerDesc.CullMode = D3D11_CULL_NONE;
		CHECK_FAILED(m_d3dDevice->CreateRasterizerState(&rasterizerDesc,
			m_rasterizeStates[(size_t)RasterizeState::CULL_NONE_WIREFRAME].GetAddressOf()));

		// 表面カリング ワイヤーフレーム
		rasterizerDesc.CullMode = D3D11_CULL_FRONT;
		CHECK_FAILED(m_d3dDevice->CreateRasterizerState(&rasterizerDesc,
			m_rasterizeStates[(size_t)RasterizeState::CULL_FRONT_WIREFRAME].GetAddressOf()));

		// 裏面カリング ワイヤーフレーム
		rasterizerDesc.CullMode = D3D11_CULL_BACK;
		CHECK_FAILED(m_d3dDevice->CreateRasterizerState(&rasterizerDesc,
			m_rasterizeStates[(size_t)RasterizeState::CULL_BACK_WIREFRAME].GetAddressOf()));

		// シャドウ用
		rasterizerDesc.FillMode					= D3D11_FILL_SOLID;
		rasterizerDesc.CullMode					= D3D11_CULL_BACK;
		rasterizerDesc.FrontCounterClockwise	= false;
		rasterizerDesc.DepthClipEnable			= true;
		rasterizerDesc.DepthBias				= 100000;
		rasterizerDesc.DepthBiasClamp			= 0.0f;
		rasterizerDesc.SlopeScaledDepthBias		= 2.0f;
		CHECK_FAILED(m_d3dDevice->CreateRasterizerState(&rasterizerDesc,
			m_rasterizeStates[(size_t)RasterizeState::SHADOW].GetAddressOf()));
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
		CHECK_FAILED(m_d3dDevice->CreateSamplerState(&samplerDesc, m_samplerStates[static_cast<size_t>(SamplerState::LINEAR_CLAMP)].GetAddressOf()));

		// ポイントクランプ
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		CHECK_FAILED(m_d3dDevice->CreateSamplerState(&samplerDesc, m_samplerStates[static_cast<size_t>(SamplerState::POINT_CLAMP)].GetAddressOf()));

		// 異方性クランプ
		samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
		CHECK_FAILED(m_d3dDevice->CreateSamplerState(&samplerDesc, m_samplerStates[static_cast<size_t>(SamplerState::ANISOTROPIC_CLAMP)].GetAddressOf()));

		// リニアラップ
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		CHECK_FAILED(m_d3dDevice->CreateSamplerState(&samplerDesc, m_samplerStates[static_cast<size_t>(SamplerState::LINEAR_WRAP)].GetAddressOf()));

		// ポイントラップ
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		CHECK_FAILED(m_d3dDevice->CreateSamplerState(&samplerDesc, m_samplerStates[static_cast<size_t>(SamplerState::POINT_WRAP)].GetAddressOf()));

		// 異方性ラップ
		samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
		CHECK_FAILED(m_d3dDevice->CreateSamplerState(&samplerDesc, m_samplerStates[static_cast<size_t>(SamplerState::ANISOTROPIC_WRAP)].GetAddressOf()));

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
		CHECK_FAILED(m_d3dDevice->CreateSamplerState(&samplerDesc, m_samplerStates[static_cast<size_t>(SamplerState::SHADOW)].GetAddressOf()));
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
		CHECK_FAILED(m_d3dDevice->CreateBlendState(&blendDesc, m_blendStates[static_cast<size_t>(BlendState::ALPHA)].GetAddressOf()));

		// 加算合成
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
		CHECK_FAILED(m_d3dDevice->CreateBlendState(&blendDesc, m_blendStates[static_cast<size_t>(BlendState::ADD)].GetAddressOf()));

		// 減算合成
		blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_REV_SUBTRACT;
		CHECK_FAILED(m_d3dDevice->CreateBlendState(&blendDesc, m_blendStates[static_cast<size_t>(BlendState::SUB)].GetAddressOf()));

		// 乗算合成
		blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ZERO;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_SRC_COLOR;
		CHECK_FAILED(m_d3dDevice->CreateBlendState(&blendDesc, m_blendStates[static_cast<size_t>(BlendState::MUL)].GetAddressOf()));

		// 反転合成
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_INV_DEST_COLOR;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
		CHECK_FAILED(m_d3dDevice->CreateBlendState(&blendDesc, m_blendStates[static_cast<size_t>(BlendState::INV)].GetAddressOf()));
	}

	// 深度ステンシルステート作成
	{
		m_depthStencilStates[static_cast<size_t>(DepthStencilState::UNKNOWN)] = nullptr;

		D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};

		depthStencilDesc.DepthEnable = true;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;

		CHECK_FAILED(m_d3dDevice->CreateDepthStencilState(&depthStencilDesc, m_depthStencilStates[static_cast<size_t>(DepthStencilState::ENABLE_TEST_AND_ENABLE_WRITE)].GetAddressOf()));

		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;

		CHECK_FAILED(m_d3dDevice->CreateDepthStencilState(&depthStencilDesc, m_depthStencilStates[static_cast<size_t>(DepthStencilState::ENABLE_TEST_AND_DISABLE_WRITE)].GetAddressOf()));

		depthStencilDesc.DepthEnable = false;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_NEVER;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;

		CHECK_FAILED(m_d3dDevice->CreateDepthStencilState(&depthStencilDesc, m_depthStencilStates[static_cast<size_t>(DepthStencilState::DISABLE_TEST_AND_ENABLE_WRITE)].GetAddressOf()));

		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;

		CHECK_FAILED(m_d3dDevice->CreateDepthStencilState(&depthStencilDesc, m_depthStencilStates[static_cast<size_t>(DepthStencilState::DISABLE_TEST_AND_DISABLE_WRITE)].GetAddressOf()));
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
				static_cast<std::size_t>(DepthStencilState::ENABLE_TEST_AND_DISABLE_WRITE)].Get(), 0);
			m_curDepthStencilState = DepthStencilState::ENABLE_TEST_AND_DISABLE_WRITE;
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
	for (auto stage = ShaderStage::VS; stage < ShaderStage::MAX; ++stage)
	{
		if (!hasStaderStage(d3dShader.m_desc.m_stages, stage)) continue;

		auto stageIndex = static_cast<std::size_t>(stage);

		// マテリアルバッファ更新
		d3dMat.m_materialBuffer._mTex = d3dMat.m_materialBuffer._mTex.Transpose();
		setCBuffer[stageIndex](m_d3dContext.Get(), SHADER::SHADER_CB_SLOT_MATERIAL, 1, m_materialBuffer.GetAddressOf());
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
		for (const auto& tex : d3dMat.m_textureData[stageIndex])
		{
			setD3D11Texture(tex.first, tex.second.id, stage);
		}

		// サンプラ更新
		for (const auto& sam : d3dMat.m_samplerData[stageIndex])
		{
			setD3D11Sampler(sam.first, sam.second.state, stage);
		}
	}

}

void D3D11RendererManager::setD3D11RenderBuffer(const RenderBufferID& renderBufferID)
{
	auto* context = m_d3dContext.Get();

	// データの取得
	auto* renderBuffer = static_cast<D3D11RenderBuffer*>(getRenderBuffer(renderBufferID));

	// 頂点バッファ更新
	if (renderBuffer->m_vertexData.isDirty)
	{
		SubResource subResource;
		d3dMap(renderBuffer->m_vertexBuffer.Get(), D3D11_MAP_WRITE_DISCARD, true, subResource);
		std::memcpy(subResource.pData, renderBuffer->m_vertexData.buffer.get(), 
			renderBuffer->m_vertexData.size * renderBuffer->m_vertexData.count);
	}

	// 頂点バッファをセット
	UINT stride = static_cast<UINT>(renderBuffer->m_vertexData.size);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, renderBuffer->m_vertexBuffer.GetAddressOf(), &stride, &offset);

	// インデックスバッファをセット
	if (renderBuffer->m_indexData.count > 0) {
		context->IASetIndexBuffer(renderBuffer->m_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	}

	// プリミティブ指定
	setD3D11PrimitiveTopology(renderBuffer->m_topology);
}

void D3D11RendererManager::setD3D11Texture(std::uint32_t slot, const TextureID& textureID, ShaderStage stage)
{
	auto stageIndex = static_cast<std::size_t>(stage);
	if (m_curTexture[stageIndex][slot] == textureID) return;


	if (textureID == NONE_TEXTURE_ID)
	{
		ID3D11ShaderResourceView* nullView = nullptr;
		setShaderResource[stageIndex](m_d3dContext.Get(), slot, 1, &nullView);
		m_curTexture[stageIndex][slot] = NONE_TEXTURE_ID;
	}
	else
	{
		D3D11Texture* pD3DTex = static_cast<D3D11Texture*>(getTexture(textureID));
		ID3D11ShaderResourceView* pTex = pD3DTex ? pD3DTex->m_srv.Get() : nullptr;
		setShaderResource[stageIndex](m_d3dContext.Get(), slot, 1, &pTex);
		m_curTexture[stageIndex][slot] = textureID;
	}
}

void D3D11RendererManager::setD3D11Sampler(std::uint32_t slot, SamplerState state, ShaderStage stage)
{
	auto stageIndex = static_cast<size_t>(stage);
	if (m_curSamplerState[stageIndex][slot] == state) {
		return;
	}

	setSamplers[stageIndex](m_d3dContext.Get() ,slot, 1, m_samplerStates[static_cast<size_t>(state)].GetAddressOf());
	m_curSamplerState[stageIndex][slot] = state;
}

void D3D11RendererManager::setD3D11PrimitiveTopology(PrimitiveTopology topology)
{
	if (m_curPrimitiveTopology == topology) return;

	m_d3dContext->IASetPrimitiveTopology(D3D11Util::getD3D11PrimitiveTopology(topology));
	m_curPrimitiveTopology = topology;
}

void D3D11RendererManager::setD3D11ShaderResourceView(std::uint32_t slot, ID3D11ShaderResourceView* srv, ShaderStage stage)
{
	auto stageIndex = static_cast<std::size_t>(stage);
	ID3D11ShaderResourceView* pTex = srv ? srv : nullptr;
	setShaderResource[stageIndex](m_d3dContext.Get(), slot, 1, &pTex);
	m_curTexture[stageIndex][slot] = NONE_TEXTURE_ID;
}

void D3D11RendererManager::setD3DSystemBuffer(const SHADER::SystemBuffer& systemBuffer)
{
	for (auto stage = ShaderStage::VS; stage < ShaderStage::MAX; ++stage)
	{
		auto stageIndex = static_cast<std::size_t>(stage);
		setCBuffer[stageIndex](m_d3dContext.Get(), SHADER::SHADER_CB_SLOT_SYSTEM, 1, m_systemBuffer.GetAddressOf());
		m_d3dContext->UpdateSubresource(m_systemBuffer.Get(), 0, nullptr, &systemBuffer, 0, 0);
	}
}

void D3D11RendererManager::setD3DTransformBuffer(const void* pData, std::uint32_t matrixCount)
{
	std::memcpy(m_transform._mWorld, pData, matrixCount * sizeof(Matrix));

	for (auto stage = ShaderStage::VS; stage < ShaderStage::MAX; ++stage)
	{
		auto stageIndex = static_cast<std::size_t>(stage);
		m_d3dContext->UpdateSubresource(m_transformBuffer.Get(), 0, nullptr, &m_transform, 0, 0);
		setCBuffer[stageIndex](m_d3dContext.Get(), SHADER::SHADER_CB_SLOT_TRANSFORM, 1, m_transformBuffer.GetAddressOf());
	}
}

void D3D11RendererManager::setD3DAnimationBuffer(std::vector<Matrix>& mtxBones)
{
	// マトリックスは倒置前提
	SubResource resource;
	std::size_t size = mtxBones.size() < SHADER::MAX_ANIMATION_BONE_COUNT ?
		mtxBones.size() * sizeof(Matrix) : SHADER::MAX_ANIMATION_BONE_COUNT * sizeof(Matrix);
	// 更新
	d3dMap(m_animationBuffer.Get(), D3D11_MAP::D3D11_MAP_WRITE_DISCARD, true, resource);
	std::memcpy(resource.pData, mtxBones.data(), size);
	d3dUnmap(m_animationBuffer.Get());
	// 指定
	setCBuffer[static_cast<std::size_t>(ShaderStage::VS)](m_d3dContext.Get(), 
		SHADER::SHADER_CB_SLOT_ANIMATION, 1, m_animationBuffer.GetAddressOf());
}

void D3D11RendererManager::setD3DLightBuffer(
	std::vector<PointLightData>& pointLights, std::vector<SpotLightData>& spotLights)
{
	SubResource resource;
	// ポイントライト
	std::size_t size = pointLights.size() < SHADER::MAX_POINT_LIGHT_COUNT ? 
		pointLights.size() * sizeof(PointLightData) : 
		SHADER::MAX_POINT_LIGHT_COUNT * sizeof(PointLightData);
	d3dMap(m_pointLightBuffer.Get(), D3D11_MAP::D3D11_MAP_WRITE_DISCARD, true, resource);
	std::memcpy(resource.pData, pointLights.data(), size);
	d3dUnmap(m_pointLightBuffer.Get());
	setShaderResource[static_cast<std::size_t>(ShaderStage::PS)](m_d3dContext.Get(), SHADER::SHADER_SRV_SLOT_POINTLIGHT, 1, m_pointLightSRV.GetAddressOf());
	// スポットライト
	size = spotLights.size() < SHADER::MAX_SPOT_LIGHT_COUNT ?
		spotLights.size() * sizeof(SpotLightData) :
		SHADER::MAX_SPOT_LIGHT_COUNT * sizeof(SpotLightData);
	d3dMap(m_spotLightBuffer.Get(), D3D11_MAP::D3D11_MAP_WRITE_DISCARD, true, resource);
	std::memcpy(resource.pData, spotLights.data(), spotLights.size() * sizeof(SpotLightData));
	d3dUnmap(m_spotLightBuffer.Get());
	setShaderResource[static_cast<std::size_t>(ShaderStage::PS)](m_d3dContext.Get(), SHADER::SHADER_SRV_SLOT_SPOTLIGHT, 1, m_spotLightSRV.GetAddressOf());

}

void D3D11RendererManager::setTexture(std::uint32_t slot, const TextureID& textureID, ShaderStage stage)
{
	setD3D11Texture(slot, textureID, stage);
}

void D3D11RendererManager::setViewport(Viewport viewport)
{
	m_d3dContext->RSSetViewports(1, viewport.Get11());
}

void D3D11RendererManager::setRenderTarget(const RenderTargetID& rtID, const DepthStencilID& dsID)
{
	setRenderTargets(1, &rtID, dsID);
}

void D3D11RendererManager::setRenderTargets(std::uint32_t num, const RenderTargetID* rtIDs, const DepthStencilID& dsID)
{
	std::vector<ID3D11RenderTargetView*> rts(num);
	for (auto i = 0u; i < num; ++i)
	{
		auto* rt = static_cast<D3D11RenderTarget*>(getRenderTarget(rtIDs[i]));
		if (rt)
			rts[i] = rt->m_rtv.Get();
		else
			rts[i] = NULL;
	}

	auto* ds = static_cast<D3D11DepthStencil*>(getDepthStencil(dsID));
	if (ds)
	{
		m_d3dContext->OMSetRenderTargets(num, rts.data(), ds->m_dsv.Get());
		m_curRTV = rts[0];
		m_curDSV = ds->m_dsv.Get();
	}
	else
	{
		m_d3dContext->OMSetRenderTargets(num, rts.data(), nullptr);
		m_curRTV = rts[0];
		m_curDSV = nullptr;
	}
}

void D3D11RendererManager::setRenderTarget(const RenderTargetID& rtID)
{
	auto* rt = static_cast<D3D11RenderTarget*>(getRenderTarget(rtID));
	if (rt)
	{
		m_d3dContext->OMSetRenderTargets(1, rt->m_rtv.GetAddressOf(), m_curDSV);
		m_curRTV = rt->m_rtv.Get();
	}
	else
	{
		m_d3dContext->OMSetRenderTargets(1, nullptr, m_curDSV);
		m_curRTV = nullptr;
	}
}
void D3D11RendererManager::setDepthStencil(const DepthStencilID& dsID)
{
	auto* ds = static_cast<D3D11DepthStencil*>(getDepthStencil(dsID));
	if (ds)
	{
		m_d3dContext->OMSetRenderTargets(1, &m_curRTV, ds->m_dsv.Get());
		m_curDSV = ds->m_dsv.Get();
	}
	else
	{
		m_d3dContext->OMSetRenderTargets(1, &m_curRTV, nullptr);
		m_curDSV = nullptr;
	}
}

void D3D11RendererManager::clearRenderTarget(const RenderTargetID& rtID)
{
	float ClearColor[4] = { 0.2f, 0.2f, 0.2f, 1.0f };
	auto* rts = static_cast<D3D11RenderTarget*>(getRenderTarget(rtID));
	if (rts)
	{
		m_d3dContext->ClearRenderTargetView(rts->m_rtv.Get(), ClearColor);
	}
}
void D3D11RendererManager::clearDepthStencil(const DepthStencilID& dsID)
{
	auto* ds = static_cast<D3D11DepthStencil*>(getDepthStencil(dsID));
	if (ds)
	{
		m_d3dContext->ClearDepthStencilView(ds->m_dsv.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}
}

void D3D11RendererManager::copyRenderTarget(const RenderTargetID& dstID, const RenderTargetID srcID)
{
	auto* pDst = static_cast<D3D11RenderTarget*>(getRenderTarget(dstID));
	auto* pSrc = static_cast<D3D11RenderTarget*>(getRenderTarget(srcID));
	if (pDst && pSrc)
	{
		d3dCopyResource(pDst->m_tex.Get(), pSrc->m_tex.Get());
	}
}

void D3D11RendererManager::d3dRender(const RenderBufferID& renderBufferID, const std::uint32_t instanceCount)
{
	auto* context = m_d3dContext.Get();

	// データの取得
	auto* renderBuffer = static_cast<D3D11RenderBuffer*>(getRenderBuffer(renderBufferID));

	// ポリゴンの描画
	if (renderBuffer->m_indexData.count > 0)
	{
		context->DrawIndexedInstanced(renderBuffer->m_indexData.count, instanceCount, 0, 0, 0);
	}
	else
	{
		context->DrawInstanced(renderBuffer->m_vertexData.count, instanceCount, 0, 0);
	}
}

void D3D11RendererManager::d3dMap(ID3D11Resource* pResource, D3D11_MAP mapType, bool mapWait, SubResource& out)
{
	D3D11_MAPPED_SUBRESOURCE subResource = {};
	UINT mapFlags = mapWait ? 0 : D3D11_MAP_FLAG_DO_NOT_WAIT;
	HRESULT hr = m_d3dContext->Map(pResource, 0, mapType, mapFlags, &subResource);
	CHECK_FAILED(hr);

	out.pData = subResource.pData;
	out.RowPitch = subResource.RowPitch;
	out.DepthPitch = subResource.DepthPitch;
}

void D3D11RendererManager::d3dUnmap(ID3D11Resource* pResource)
{
	m_d3dContext->Unmap(pResource, 0);
}

void D3D11RendererManager::d3dCopyResource(ID3D11Resource* pDst, ID3D11Resource* pSrc)
{
	m_d3dContext->CopyResource(pDst, pSrc);
}

void D3D11RendererManager::d3dGenerateMips(const RenderTargetID& dstID)
{
	auto* rt = static_cast<D3D11RenderTarget*>(getRenderTarget(dstID));

	m_d3dContext->GenerateMips(rt->m_srv.Get());
}

AnimationID D3D11RendererManager::createAnimation(std::string name)
{
	// IDの取得
	AnimationID id = hash::crc32string(name.c_str());

	// 既に生成済み
	const auto& itr = m_animationPool.find(id);
	if (m_animationPool.end() != itr) return id;

	// 新規生成
	m_animationPool[id] = std::make_unique<Animation>(id, name);

	return id;
}

BufferID D3D11RendererManager::createBuffer(BufferDesc desc, BufferData* pData)
{
	// IDの取得
	BufferID id = hash::crc32string(desc.name.c_str());

	// 既に生成済み
	const auto& itr = m_bufferPool.find(id);
	if (m_bufferPool.end() != itr) return id;

	// 新規生成
	m_bufferPool[id] = std::make_unique<D3D11Buffer>(m_d3dDevice.Get(), id, desc, pData);

	return id;
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
	RenderBufferID id = shaderID ^ meshID;
	//std::memcpy(&id, &shaderID, sizeof(std::uint32_t));
	//std::memcpy(&id + sizeof(std::uint32_t), &meshID, sizeof(std::uint32_t));
	
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

TextureID D3D11RendererManager::createTextureFromFile(std::string filePath)
{
	// IDの取得
	TextureID id = hash::crc32string(filePath.c_str());

	// 既に生成済み
	const auto& itr = m_texturePool.find(id);
	if (m_texturePool.end() != itr) return id;

	// 新規生成
	auto pTex = std::make_unique<D3D11Texture>(id, filePath);

	// テクスチャの読み込み
	HRESULT hr = pTex->CreateFromFile(m_d3dDevice.Get(), m_d3dContext.Get());
	if (FAILED(hr))
	{
		pTex.release();
		return NONE_TEXTURE_ID;
	}

	// 成功したら
	m_texturePool[id] = std::move(pTex);

	return id;
}

TextureID D3D11RendererManager::createTextureFromMemory(std::string name, 
	const std::uint8_t* wicData, std::size_t wicDataSize)
{
	// IDの取得
	TextureID id = hash::crc32string(name.c_str());

	// 既に生成済み
	const auto& itr = m_texturePool.find(id);
	if (m_texturePool.end() != itr) return id;

	// 新規生成
	auto pTex = std::make_unique<D3D11Texture>(id, name);

	// テクスチャの読み込み
	HRESULT hr = pTex->CreateFromMemory(m_d3dDevice.Get(), wicData, wicDataSize);
	if (FAILED(hr))
	{
		pTex.release();
		return NONE_TEXTURE_ID;
	}

	// 成功したら
	m_texturePool[id] = std::move(pTex);

	return id;
}

RenderTargetID D3D11RendererManager::createRenderTarget(std::string name, bool bMipMap)
{
	// IDの取得
	RenderTargetID id = hash::crc32string(name.c_str());

	// 既に生成済み
	const auto& itr = m_renderTargetPool.find(id);
	if (m_renderTargetPool.end() != itr) return id;

	D3D11RenderTargetDesc desc;
	desc.id = id;
	desc.name = name;
	desc.format = m_backBufferFormat;
	desc.width = m_pEngine->getWindowWidth();
	desc.height = m_pEngine->getWindowHeight();
	desc.isMSAA = m_bUseMSAA;
	desc.sampleDesc = m_dxgiMSAA;
	desc.isMipMap = bMipMap;

	// 新規生成
	m_renderTargetPool[id] = std::make_unique<D3D11RenderTarget>(m_d3dDevice.Get(), desc);

	return id;
}

DepthStencilID D3D11RendererManager::createDepthStencil(std::string name)
{
	// IDの取得
	DepthStencilID id = hash::crc32string(name.c_str());

	// 既に生成済み
	const auto& itr = m_depthStencilPool.find(id);
	if (m_depthStencilPool.end() != itr) return id;

	D3D11DepthStencilDesc desc;
	desc.id = id;
	desc.name = name;
	desc.format = m_depthStencilFormat;
	desc.width = m_pEngine->getWindowWidth();
	desc.height = m_pEngine->getWindowHeight();
	desc.isMSAA = m_bUseMSAA;
	desc.sampleDesc = m_dxgiMSAA;

	// 新規生成
	m_depthStencilPool[id] = std::make_unique<D3D11DepthStencil>(m_d3dDevice.Get(), desc);

	return id;
}

BatchGroupID D3D11RendererManager::creatBatchGroup(MaterialID materialID, MeshID meshID)
{
	// IDの生成
	BatchGroupID id = materialID ^ meshID;
	//std::memcpy(&id, &materialID, sizeof(std::uint32_t));
	//std::memcpy(&id + sizeof(std::uint32_t), &meshID, sizeof(std::uint32_t));

	// 既にある
	const auto& itr = m_batchGroupPool.find(id);
	if (m_batchGroupPool.end() != itr) return id;

	// 新規生成
	m_batchGroupPool[id] = std::make_unique<BatchGroup>(id, materialID, meshID);

	return id;
}


SubResource D3D11RendererManager::mapTexture(TextureID texID)
{
	SubResource subResource;
	auto* pTex = getTexture(texID);
	if (pTex == nullptr) return subResource;

	auto* pD3DTex = static_cast<D3D11Texture*>(pTex);
	d3dMap(pD3DTex->m_tex.Get(), D3D11_MAP_READ, true, subResource);

	D3D11_TEXTURE2D_DESC tex;
	pD3DTex->m_tex->GetDesc(&tex);
	subResource.ByteWidth = DirectX::BitsPerPixel(tex.Format) / 8;

	return subResource;
}
SubResource D3D11RendererManager::mapVertex(RenderBufferID rdID)
{
	SubResource subResource;
	auto* pRD = getRenderBuffer(rdID);
	if (pRD == nullptr) return subResource;

	auto* pD3DRd = static_cast<D3D11RenderBuffer*>(pRD);
	d3dMap(pD3DRd->m_vertexBuffer.Get(), D3D11_MAP_WRITE_DISCARD, true, subResource);

	D3D11_BUFFER_DESC vertex;
	pD3DRd->m_vertexBuffer->GetDesc(&vertex);
	subResource.ByteWidth = vertex.ByteWidth;

	return subResource;
}

void D3D11RendererManager::unmapTexture(TextureID texID)
{
	auto* pTex = getTexture(texID);
	if (pTex == nullptr) return;

	auto* pD3DTex = static_cast<D3D11Texture*>(pTex);
	d3dUnmap(pD3DTex->m_tex.Get());
}
void D3D11RendererManager::unmapVertex(RenderBufferID rdID)
{
	auto* pRD = getRenderBuffer(rdID);
	if (pRD == nullptr) return;

	auto* pD3DRd = static_cast<D3D11RenderBuffer*>(pRD);
	d3dUnmap(pD3DRd->m_vertexBuffer.Get());
}
