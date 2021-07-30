/*****************************************************************//**
 * \file   D3D11RendererManager.h
 * \brief  DirectX11�����_���[
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
#include "D3D11Texture.h"

//#ifdef _DEBUG
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "ImGuizmo.h"
//#include "ImGuiUtils.h"
//#endif

// ���C�u���������N
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
		setCBuffer[static_cast<std::size_t>(EShaderStage::MAX)] = {
		&ID3D11DeviceContext1::VSSetConstantBuffers,
		&ID3D11DeviceContext1::GSSetConstantBuffers,
		&ID3D11DeviceContext1::DSSetConstantBuffers,
		&ID3D11DeviceContext1::HSSetConstantBuffers,
		&ID3D11DeviceContext1::PSSetConstantBuffers,
		&ID3D11DeviceContext1::CSSetConstantBuffers, };
	// SRV
	static std::function<void(ID3D11DeviceContext1*, UINT, UINT, ID3D11ShaderResourceView* const*)>
		setShaderResource[static_cast<std::size_t>(EShaderStage::MAX)] = {
		&ID3D11DeviceContext1::VSSetShaderResources,
		&ID3D11DeviceContext1::GSSetShaderResources,
		&ID3D11DeviceContext1::DSSetShaderResources,
		&ID3D11DeviceContext1::HSSetShaderResources,
		&ID3D11DeviceContext1::PSSetShaderResources,
		&ID3D11DeviceContext1::CSSetShaderResources, };
	// Sampler
	static std::function<void(ID3D11DeviceContext1*, UINT, UINT, ID3D11SamplerState* const*)>
		setSamplers[static_cast<std::size_t>(EShaderStage::MAX)] = {
		&ID3D11DeviceContext1::VSSetSamplers,
		&ID3D11DeviceContext1::GSSetSamplers,
		&ID3D11DeviceContext1::DSSetSamplers,
		&ID3D11DeviceContext1::HSSetSamplers,
		&ID3D11DeviceContext1::PSSetSamplers,
		&ID3D11DeviceContext1::CSSetSamplers, };

	D3D11_PRIMITIVE_TOPOLOGY getD3D11PrimitiveTopology(EPrimitiveTopology topology) {
		static D3D11_PRIMITIVE_TOPOLOGY d3dTopologies[static_cast<size_t>(EPrimitiveTopology::MAX)] = {
			D3D_PRIMITIVE_TOPOLOGY_UNDEFINED,
			D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
			D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
			D3D_PRIMITIVE_TOPOLOGY_POINTLIST,
			D3D_PRIMITIVE_TOPOLOGY_LINELIST,
			D3D_PRIMITIVE_TOPOLOGY_LINESTRIP,
			D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST
		};

		return d3dTopologies[static_cast<size_t>(topology)];
	}
}


/// @brief �R���X�g���N�^
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
	m_curRasterizeState(ERasterizeState::CULL_NONE),
	m_curDepthStencilState(EDepthStencilState::DISABLE_TEST_AND_DISABLE_WRITE)
{
}

/// @brief �R���X�g���N�^
D3D11RendererManager::~D3D11RendererManager()
{
}

/// @brief ������
/// @param hWnd �E�B���h�E�n���h��
/// @param width �E�B���h�E�̕�
/// @param height �E�B���h�E�̍���
/// @return ���� TRUE ���s FALSE
HRESULT D3D11RendererManager::initialize(HWND hWnd, int width, int height)
{
	// ������
	m_hWnd = hWnd;
	m_nOutputWidth = width;
	m_nOutputHeight = height;

	// �f�o�C�X�̏�����
	CHECK_FAILED(createDivece());

	// �X���b�v�`�F�[���ƃo�b�t�@�̏�����
	CHECK_FAILED(createSwapChainAndBuffer());

	// ���ʃX�e�[�g�̏�����
	CHECK_FAILED(createCommonState());

	// ����CBuffer�̏�����
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

	// Sampler
	for (auto stage = EShaderStage::VS; stage < EShaderStage::MAX; ++stage) 
	{
		setD3D11Sampler(D3D::SHADER_SS_SLOT_MAIN,	ESamplerState::LINEAR_WRAP,		 stage);
		setD3D11Sampler(D3D::SHADER_SS_SLOT_SHADOW, ESamplerState::SHADOW,			 stage);
		setD3D11Sampler(D3D::SHADER_SS_SLOT_SKYBOX, ESamplerState::ANISOTROPIC_WRAP, stage);
	}


	//--- imgui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;	// �E�B���h�E�ɂȂ�
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

	return S_OK;
}

/// @brief �I������
void D3D11RendererManager::finalize()
{
	//--- imgui
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

}

/// @brief �o�b�t�@�N���A
void D3D11RendererManager::clear()
{
	//--- imgui
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	ImGuizmo::BeginFrame();

	// debug�\��
	imguiDebug();

	// �o�b�t�@�̃N���A
	m_d3dAnnotation->BeginEvent(L"Clear");

	m_d3dContext->OMSetBlendState(nullptr, nullptr, std::numeric_limits<std::uint32_t>::max());

	float ClearColor[4] = { 0.117647f, 0.254902f, 0.352941f, 1.0f };
	m_d3dContext->ClearRenderTargetView(m_backBufferRTV.Get(), ClearColor);
	m_d3dContext->ClearRenderTargetView(m_diffuseRTV.Get(), ClearColor);
	m_d3dContext->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	m_d3dContext->OMSetRenderTargets(1, m_backBufferRTV.GetAddressOf(), m_depthStencilView.Get());
	//m_d3dContext->OMSetRenderTargets(1, m_diffuseRTV.GetAddressOf(), m_depthStencilView.Get());

	m_d3dContext->RSSetViewports(1, &m_vireport);

	m_d3dAnnotation->EndEvent();
}

/// @brief �X���b�v
void D3D11RendererManager::present()
{
	// �o�b�N�o�b�t�@�ɖ߂�
	//m_d3dContext->OMSetRenderTargets(1, m_backBufferRTV.GetAddressOf(), m_depthStencilView.Get());

	//--- imgui
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}

	//m_swapChain->Present(0, DXGI_PRESENT_ALLOW_TEARING); // �e�B�A�����O���e�`��
	//m_swapChain->Present(0, 0);		// �񐂒������`��
	m_swapChain->Present(1, 0);	// ���������`��
}

/// @brief Imgui�\��
void D3D11RendererManager::imguiDebug()
{
	// �S�̃E�B���h�E
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
	//// ���j���[�o�[
	//if (ImGui::BeginMainMenuBar())
	//{
	//	if (ImGui::BeginMenu("File"))
	//	{

	//		ImGui::EndMenu();
	//	}

	//	ImGui::EndMainMenuBar();
	//}
	//ImGui::End();


	//// Scene
	//ImGui::SetNextWindowBgAlpha(0.0f);
	//ImGui::Begin("Scene",0, ImGuiWindowFlags_::ImGuiWindowFlags_NoMove | 
	//	ImGuiWindowFlags_::ImGuiWindowFlags_NoTitleBar);
	////ImGui::Image(m_diffuseSRV.Get(), ImVec2(m_pEngine->getWindowWidth() * 0.6f, 
	//	//m_pEngine->getWindowHeight() * 0.6f));
	//ImGui::End();


	// �}�e���A��
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
				bool temp = d3dMat->m_materialBuffer._Flg & (UINT)D3D::Material_Flg::TEXTURE;
				if (ImGui::Checkbox("Texture", &temp))
					d3dMat->m_materialBuffer._Flg ^= (UINT)D3D::Material_Flg::TEXTURE;
				// Light
				temp = d3dMat->m_materialBuffer._Flg & (UINT)D3D::Material_Flg::LIGHT;
				if (ImGui::Checkbox("Lighting", &temp))
					d3dMat->m_materialBuffer._Flg ^= (UINT)D3D::Material_Flg::LIGHT;
				// Shadow
				temp = d3dMat->m_materialBuffer._Flg & (UINT)D3D::Material_Flg::SHADOW;
				if (ImGui::Checkbox("Shadow", &temp))
					d3dMat->m_materialBuffer._Flg ^= (UINT)D3D::Material_Flg::SHADOW;
				// Fog
				temp = d3dMat->m_materialBuffer._Flg & (UINT)D3D::Material_Flg::FOG;
				if (ImGui::Checkbox("Fog", &temp))
					d3dMat->m_materialBuffer._Flg ^= (UINT)D3D::Material_Flg::FOG;
			}
			//Color
			ImGui::SliderFloat4("Color", (float*)&d3dMat->m_materialBuffer._Color, 0.0f, 1.0f);
			//ImGui::InputText("textbox 1", text1, sizeof(text1));

			// CBuffer�ϐ�
			for (const auto& var : mat.second->m_cbufferVariable)
			{
				float temp = *(float*)mat.second->getData(var.second.name.c_str());
				if (ImGui::SliderFloat(var.second.name.c_str(), &temp, 0.0f, 1.0f))
					mat.second->setFloat(var.second.name.c_str(), temp);
			}

			for(auto stage = EShaderStage::VS; stage < EShaderStage::MAX; ++stage)
			{
				auto stageIndex = static_cast<size_t>(stage);
				// �e�N�X�`��
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

				// �T���v���[


			}

			ImGui::TreePop();
		}
	}

	ImGui::End();
}

/// @brief �f�o�C�X�̐���
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

	// �t�@�N�g���[�̎擾
	hr = CreateDXGIFactory1(__uuidof(IDXGIFactory), (void**)(&m_dxgiFactory));
	if (FAILED(hr)) {
		MessageBoxW(hWnd, L"CreateDXGIFactory1", L"Err", MB_ICONSTOP);
		return hr;
	}
	// �A�_�v�^�[�̎擾
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
		// �` �A�_�v�^�̑I��
		aiAdapter.push_back(iAdapter);
	}
	aiAdapter.pop_back();
	if (aiAdapter.size() <= 0)
	{
		aiAdapter.push_back(NULL);
	}


	//--- �f�o�C�X�̐��� ---
	ComPtr<ID3D11Device>		device;
	ComPtr<ID3D11DeviceContext> context;
	ComPtr<ID3D11DeviceContext> defferedContext;

	// �f�o�C�X�B�R���e�L�X�g�̐���
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

	// �x���R���e�L�X�g�쐬
	hr = device->CreateDeferredContext(0, defferedContext.GetAddressOf());
	if (FAILED(hr)) {
		MessageBoxW(hWnd, L"CreateDeferredContext", L"Err", MB_ICONSTOP);
		return hr;
	}

	// �i�[
	hr = device.As(&m_d3dDevice);
	hr = context.As(&m_d3dContext);
	hr = context.As(&m_d3dAnnotation);
	hr = defferedContext.As(&m_d3dDefferedContext);
	if (FAILED(hr)) {
		MessageBoxW(hWnd, L"device,context,defferedContext", L"Err", MB_ICONSTOP);
		return hr;
	}

	//�g�p�\��MSAA���擾
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

/// @brief �X���b�v�`�F�[���ƃo�b�t�@�̐���
/// @return HRESULT
HRESULT D3D11RendererManager::createSwapChainAndBuffer()
{
	HRESULT hr;
	HWND hWnd = m_hWnd;

	// �����_�[�^�[�Q�b�g�̃T�C�Y�w��
	std::uint32_t backBufferWidth =
		std::max<std::uint32_t>(static_cast<std::uint32_t>(m_nOutputWidth), 1);
	std::uint32_t backBufferHeight =
		std::max<std::uint32_t>(static_cast<std::uint32_t>(m_nOutputHeight), 1);

	//--- �X���b�v�`�F�C�� ---
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
	// �t���X�N���[���w��
	DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsSwapChainDesc = {};
	fsSwapChainDesc.Windowed = TRUE;

	// �X���b�v�`�F�[���̍쐬
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

	// �r���I�t���X�N���[�����[�h�𖳌���
	hr = m_dxgiFactory->MakeWindowAssociation(m_hWnd, DXGI_MWA_NO_ALT_ENTER);
	if (FAILED(hr)) {
		MessageBoxW(hWnd, L"MakeWindowAssociation", L"Err", MB_ICONSTOP);
		return hr;
	}


	//--- �o�b�N�o�b�t�@RT ---

	// �t�H�[�}�b�g�T�|�[�g�`�F�b�N
	std::uint32_t formatSupport = 0;
	if (FAILED(m_d3dDevice->CheckFormatSupport(m_backBufferFormat, &formatSupport)))
	{
		MessageBoxW(hWnd, L"CheckFormatSupport", L"Err", MB_ICONSTOP);
		return E_FAIL;
	}

	// �o�b�N�o�b�t�@�����_�[�^�[�Q�b�g�̍쐬
	m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
		(LPVOID*)m_backBufferRT.ReleaseAndGetAddressOf());

	CD3D11_RENDER_TARGET_VIEW_DESC rtvDesc(D3D11_RTV_DIMENSION_TEXTURE2D, m_backBufferFormat);
	if (m_bUseMSAA) rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
	// �����_�[�^�[�Q�b�g�r���[�쐬
	hr = m_d3dDevice->CreateRenderTargetView(
		m_backBufferRT.Get(),
		&rtvDesc,
		m_backBufferRTV.ReleaseAndGetAddressOf());
	if (FAILED(hr)) {
		MessageBoxW(hWnd, L"CreateRenderTargetView", L"Err", MB_ICONSTOP);
		return hr;
	}

	
	//--- �g�U���ˌ��o�b�t�@
	// �e�N�X�`��2D
	D3D11_TEXTURE2D_DESC renderTexture;
	m_backBufferRT->GetDesc(&renderTexture);
	renderTexture.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
	hr = m_d3dDevice->CreateTexture2D(
		&renderTexture,
		nullptr,
		m_diffuseRT.ReleaseAndGetAddressOf()
	);
	CHECK_FAILED(hr);
	// �����_���[�^�[�Q�b�g�r���[
	hr = m_d3dDevice->CreateRenderTargetView(
		m_diffuseRT.Get(),
		&rtvDesc,
		m_diffuseRTV.ReleaseAndGetAddressOf());
	// �V�F�[�_�[���\�[�X
	CD3D11_SHADER_RESOURCE_VIEW_DESC srvDesc(D3D11_SRV_DIMENSION_TEXTURE2D, m_backBufferFormat);
	if (m_bUseMSAA) srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
	hr = m_d3dDevice->CreateShaderResourceView(
		m_diffuseRT.Get(),
		NULL,
		m_diffuseSRV.ReleaseAndGetAddressOf()
	);


	//--- �[�x�X�e���V�� ---
	CD3D11_TEXTURE2D_DESC dsDesc(m_depthStencilFormat, backBufferWidth, backBufferHeight,
		1, 1, D3D11_BIND_DEPTH_STENCIL);
	if (m_bUseMSAA) dsDesc.SampleDesc = m_dxgiMSAA;
	// �[�x�X�e���V���e�N�X�`���쐬
	hr = m_d3dDevice->CreateTexture2D(&dsDesc, nullptr,
		m_depthStencilTexture.ReleaseAndGetAddressOf());
	CHECK_FAILED(hr);
	// �[�x�X�e���V���e�N�X�`���r���[
	CD3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc(D3D11_DSV_DIMENSION_TEXTURE2D);
	if (m_bUseMSAA) dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
	hr = m_d3dDevice->CreateDepthStencilView(m_depthStencilTexture.Get(),
		&dsvDesc, m_depthStencilView.ReleaseAndGetAddressOf());
	CHECK_FAILED(hr);


	// �r���[�|�[�g
	m_vireport = CD3D11_VIEWPORT(0.0f, 0.0f,
		static_cast<float>(backBufferWidth),
		static_cast<float>(backBufferHeight));

	return S_OK;
}

/// @brief ���ʃX�e�[�g�̐���
/// @return HRESULT
HRESULT D3D11RendererManager::createCommonState()
{
	// ���X�^���C�U�X�e�[�g�쐬
	{
		D3D11_RASTERIZER_DESC rasterizerDesc = {};

		// �J�����O�Ȃ� �h��Ԃ�
		rasterizerDesc.FillMode = D3D11_FILL_SOLID;
		rasterizerDesc.CullMode = D3D11_CULL_NONE;
		CHECK_FAILED(m_d3dDevice->CreateRasterizerState(&rasterizerDesc,
			m_rasterizeStates[(size_t)ERasterizeState::CULL_NONE].GetAddressOf()));

		// �\�ʃJ�����O �h��Ԃ�
		rasterizerDesc.CullMode = D3D11_CULL_FRONT;
		CHECK_FAILED(m_d3dDevice->CreateRasterizerState(&rasterizerDesc,
			m_rasterizeStates[(size_t)ERasterizeState::CULL_FRONT].GetAddressOf()));

		// ���ʃJ�����O �h��Ԃ�
		rasterizerDesc.CullMode = D3D11_CULL_BACK;
		CHECK_FAILED(m_d3dDevice->CreateRasterizerState(&rasterizerDesc,
			m_rasterizeStates[(size_t)ERasterizeState::CULL_BACK].GetAddressOf()));

		// �J�����O�Ȃ� ���C���[�t���[��
		rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
		rasterizerDesc.CullMode = D3D11_CULL_NONE;
		CHECK_FAILED(m_d3dDevice->CreateRasterizerState(&rasterizerDesc,
			m_rasterizeStates[(size_t)ERasterizeState::CULL_NONE_WIREFRAME].GetAddressOf()));

		// �\�ʃJ�����O ���C���[�t���[��
		rasterizerDesc.CullMode = D3D11_CULL_FRONT;
		CHECK_FAILED(m_d3dDevice->CreateRasterizerState(&rasterizerDesc,
			m_rasterizeStates[(size_t)ERasterizeState::CULL_FRONT_WIREFRAME].GetAddressOf()));

		// ���ʃJ�����O ���C���[�t���[��
		rasterizerDesc.CullMode = D3D11_CULL_BACK;
		CHECK_FAILED(m_d3dDevice->CreateRasterizerState(&rasterizerDesc,
			m_rasterizeStates[(size_t)ERasterizeState::CULL_BACK_WIREFRAME].GetAddressOf()));

		// �V���h�E�p
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

	// �T���v���X�e�[�g�쐬
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

		// ���j�A�N�����v
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		CHECK_FAILED(m_d3dDevice->CreateSamplerState(&samplerDesc, m_samplerStates[static_cast<size_t>(ESamplerState::LINEAR_CLAMP)].GetAddressOf()));

		// �|�C���g�N�����v
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		CHECK_FAILED(m_d3dDevice->CreateSamplerState(&samplerDesc, m_samplerStates[static_cast<size_t>(ESamplerState::POINT_CLAMP)].GetAddressOf()));

		// �ٕ����N�����v
		samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
		CHECK_FAILED(m_d3dDevice->CreateSamplerState(&samplerDesc, m_samplerStates[static_cast<size_t>(ESamplerState::ANISOTROPIC_CLAMP)].GetAddressOf()));

		// ���j�A���b�v
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		CHECK_FAILED(m_d3dDevice->CreateSamplerState(&samplerDesc, m_samplerStates[static_cast<size_t>(ESamplerState::LINEAR_WRAP)].GetAddressOf()));

		// �|�C���g���b�v
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		CHECK_FAILED(m_d3dDevice->CreateSamplerState(&samplerDesc, m_samplerStates[static_cast<size_t>(ESamplerState::POINT_WRAP)].GetAddressOf()));

		// �ٕ������b�v
		samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
		CHECK_FAILED(m_d3dDevice->CreateSamplerState(&samplerDesc, m_samplerStates[static_cast<size_t>(ESamplerState::ANISOTROPIC_WRAP)].GetAddressOf()));

		// �V���h�E
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

	// �u�����h�X�e�[�g�쐬
	{
		D3D11_BLEND_DESC blendDesc = {};

		blendDesc.AlphaToCoverageEnable = false;
		blendDesc.IndependentBlendEnable = false;
		blendDesc.RenderTarget[0].BlendEnable = true;
		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		// ����������
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		CHECK_FAILED(m_d3dDevice->CreateBlendState(&blendDesc, m_blendStates[static_cast<size_t>(EBlendState::ALPHA)].GetAddressOf()));

		// ���Z����
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
		CHECK_FAILED(m_d3dDevice->CreateBlendState(&blendDesc, m_blendStates[static_cast<size_t>(EBlendState::ADD)].GetAddressOf()));

		// ���Z����
		blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_REV_SUBTRACT;
		CHECK_FAILED(m_d3dDevice->CreateBlendState(&blendDesc, m_blendStates[static_cast<size_t>(EBlendState::SUB)].GetAddressOf()));

		// ��Z����
		blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ZERO;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_SRC_COLOR;
		CHECK_FAILED(m_d3dDevice->CreateBlendState(&blendDesc, m_blendStates[static_cast<size_t>(EBlendState::MUL)].GetAddressOf()));

		// ���]����
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_INV_DEST_COLOR;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
		CHECK_FAILED(m_d3dDevice->CreateBlendState(&blendDesc, m_blendStates[static_cast<size_t>(EBlendState::INV)].GetAddressOf()));
	}

	// �[�x�X�e���V���X�e�[�g�쐬
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

/// @brief D3D11�}�e���A���̎w��
/// @param materialID �}�e���A��ID
void D3D11RendererManager::setD3D11Material(const MaterialID& materialID)
{
	// �}�e���A���̎擾
	auto* d3dMat = static_cast<D3D11Material*>(getMaterial(materialID));
	if (d3dMat == nullptr) return;

	// �V�F�[�_�[�̎擾
	auto* d3dShader = static_cast<D3D11Shader*>(getShader(d3dMat->m_shaderID));
	if (d3dShader == nullptr) return;

	// �V�F�[�_�[�̎w��
	if (m_curD3DShader != d3dShader)
	{
		if (d3dShader->vs) m_d3dContext->VSSetShader(d3dShader->vs, nullptr, 0);
		if (d3dShader->gs) m_d3dContext->GSSetShader(d3dShader->gs, nullptr, 0);
		if (d3dShader->ds) m_d3dContext->DSSetShader(d3dShader->ds, nullptr, 0);
		if (d3dShader->hs) m_d3dContext->HSSetShader(d3dShader->hs, nullptr, 0);
		if (d3dShader->ps) m_d3dContext->PSSetShader(d3dShader->ps, nullptr, 0);
		if (d3dShader->cs) m_d3dContext->CSSetShader(d3dShader->cs, nullptr, 0);
		// ���̓��C�A�E�g�w��
		m_d3dContext->IASetInputLayout(d3dShader->m_inputLayout.Get());
		m_curD3DShader = d3dShader;
	}

	// �u�����h�X�e�[�g
	if (m_curBlendState != d3dMat->m_blendState)
	{
		constexpr float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
		m_d3dContext->OMSetBlendState(m_blendStates[
			static_cast<std::size_t>(d3dMat->m_blendState)].Get(), blendFactor, 0xffffffff);
		m_curBlendState = d3dMat->m_blendState;
	}
	// ���X�^���C�U�X�e�[�g
	if (m_curRasterizeState != d3dMat->m_rasterizeState)
	{
		m_d3dContext->RSSetState(m_rasterizeStates[
			static_cast<std::size_t>(d3dMat->m_rasterizeState)].Get());
		m_curRasterizeState = d3dMat->m_rasterizeState;
	}
	// �[�x�X�e���V���X�e�[�g
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

	// �}�e���A�����\�[�X�w��E�X�V
	setD3D11MaterialResource(*d3dMat, *d3dShader);

}

/// @brief D3D11�}�e���A���̃��\�[�X���w��
/// @param d3dMaterial D3D11�}�e���A���̎Q��
void D3D11RendererManager::setD3D11MaterialResource(const D3D11Material& d3dMaterial, const D3D11Shader& d3dShader)
{
	auto& d3dMat = const_cast<D3D11Material&>(d3dMaterial);

	// �X�e�[�W����
	for (auto stage = EShaderStage::VS; stage < EShaderStage::MAX; ++stage)
	{
		if (!hasStaderStage(d3dShader.m_desc.m_stages, stage)) continue;

		auto stageIndex = static_cast<std::size_t>(stage);

		// �}�e���A���o�b�t�@�X�V
		setCBuffer[stageIndex](m_d3dContext.Get(), D3D::SHADER_CB_SLOT_MATERIAL, 1, m_materialBuffer.GetAddressOf());
		m_d3dContext->UpdateSubresource(m_materialBuffer.Get(), 0, nullptr, &d3dMat.m_materialBuffer, 0, 0);

		// �R���X�^���g�o�b�t�@
		for (const auto& cbuffer : d3dMaterial.m_d3dCbuffer[stageIndex]) 
		{
			// �w��
			setCBuffer[stageIndex](m_d3dContext.Get(), cbuffer.first, 1, cbuffer.second.GetAddressOf());
			// �X�V
			if (d3dMat.m_cbufferData[stageIndex][cbuffer.first].isUpdate)
			{
				m_d3dContext->UpdateSubresource(cbuffer.second.Get(), 0, nullptr,
					d3dMat.m_cbufferData[stageIndex][cbuffer.first].data.get(), 0, 0);
				d3dMat.m_cbufferData[stageIndex][cbuffer.first].isUpdate = false;
			}
		}

		// �e�N�X�`���X�V
		for (const auto& tex : d3dMat.m_textureData[stageIndex])
		{
			setD3D11Texture(tex.first, tex.second.id, stage);
		}

		// �T���v���X�V
		for (const auto& sam : d3dMat.m_samplerData[stageIndex])
		{
			setD3D11Sampler(sam.first, sam.second.state, stage);
		}
	}

}

void D3D11RendererManager::setD3D11RenderBuffer(const RenderBufferID& renderBufferID)
{
	auto* context = m_d3dContext.Get();

	// �f�[�^�̎擾
	auto* renderBuffer = static_cast<D3D11RenderBuffer*>(getRenderBuffer(renderBufferID));

	// ���_�o�b�t�@���Z�b�g
	UINT stride = static_cast<UINT>(renderBuffer->m_vertexData.size);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, renderBuffer->m_vertexBuffer.GetAddressOf(), &stride, &offset);
	// �C���f�b�N�X�o�b�t�@���Z�b�g
	if (renderBuffer->m_indexData.count > 0) {
		context->IASetIndexBuffer(renderBuffer->m_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	}

	// �v���~�e�B�u�w��
	setD3D11PrimitiveTopology(renderBuffer->m_topology);
}

void D3D11RendererManager::setD3D11Texture(std::uint32_t slot, const TextureID& textureID, EShaderStage stage)
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

void D3D11RendererManager::setD3D11Sampler(std::uint32_t slot, ESamplerState state, EShaderStage stage)
{
	auto stageIndex = static_cast<size_t>(stage);
	if (m_curSamplerState[stageIndex][slot] == state) {
		return;
	}

	setSamplers[stageIndex](m_d3dContext.Get() ,slot, 1, m_samplerStates[static_cast<size_t>(state)].GetAddressOf());
	m_curSamplerState[stageIndex][slot] = state;
}

void D3D11RendererManager::setD3D11PrimitiveTopology(EPrimitiveTopology topology)
{
	if (m_curPrimitiveTopology == topology) return;

	m_d3dContext->IASetPrimitiveTopology(getD3D11PrimitiveTopology(topology));
	m_curPrimitiveTopology = topology;
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


void D3D11RendererManager::setTexture(std::uint32_t slot, const TextureID& textureID, EShaderStage stage)
{
	setD3D11Texture(slot, textureID, stage);
}


void D3D11RendererManager::d3dRender(const RenderBufferID& renderBufferID)
{
	auto* context = m_d3dContext.Get();

	// �f�[�^�̎擾
	auto* renderBuffer = static_cast<D3D11RenderBuffer*>(getRenderBuffer(renderBufferID));

	// �|���S���̕`��
	if (renderBuffer->m_indexData.count > 0)
	{
		context->DrawIndexed(renderBuffer->m_indexData.count, 0, 0);
	}
	else
	{
		context->Draw(renderBuffer->m_vertexData.count, 0);
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


ShaderID D3D11RendererManager::createShader(ShaderDesc desc)
{
	// ID�̎擾
	ShaderID id = hash::crc32string(desc.m_name.c_str());

	// ���ɐ����ς�
	const auto& itr = m_shaderPool.find(id);
	if (m_shaderPool.end() != itr) return id;

	// �V�K����
	m_shaderPool[id] = std::make_unique<D3D11Shader>(m_d3dDevice.Get(), desc, id);

	return id;
}

MaterialID D3D11RendererManager::createMaterial(std::string name, ShaderID shaderID)
{
	// ID�̎擾
	MaterialID id = hash::crc32string(name.c_str());

	// ���ɐ����ς�
	const auto& itr = m_materialPool.find(id);
	if (m_materialPool.end() != itr) return id;

	// �V�F�[�_�[�擾
	auto* shader = getShader(shaderID);
	if (shader == nullptr) return NONE_MATERIAL_ID;

	// �V�K����
	m_materialPool[id] = std::make_unique<D3D11Material>(
		m_d3dDevice.Get(), id, name, *shader);

	return id;
}

MeshID D3D11RendererManager::createMesh(std::string name)
{
	// ID�̎擾
	MeshID id = hash::crc32string(name.c_str());

	// ���ɐ����ς�
	const auto& itr = m_meshPool.find(id);
	if (m_meshPool.end() != itr) return id;

	// �V�K����
	m_meshPool[id] = std::make_unique<Mesh>(id, name);

	return id;
}

RenderBufferID D3D11RendererManager::createRenderBuffer(ShaderID shaderID, MeshID meshID)
{
	// ID�̐���
	RenderBufferID id = shaderID ^ meshID;
	//std::memcpy(&id, &shaderID, sizeof(std::uint32_t));
	//std::memcpy(&id + sizeof(std::uint32_t), &meshID, sizeof(std::uint32_t));
	
	// ���ɂ���
	const auto& itr = m_renderBufferPool.find(id);
	if (m_renderBufferPool.end() != itr) return id;

	// �V�F�[�_�[�擾
	auto* shader = getShader(shaderID);
	if (shader == nullptr) return NONE_RENDERBUFFER_ID;
	// ���b�V���擾
	auto* mesh = getMesh(meshID);
	if (mesh == nullptr) return NONE_RENDERBUFFER_ID;

	// �V�K����
	m_renderBufferPool[id] = std::make_unique<D3D11RenderBuffer>(
		m_d3dDevice.Get(), id, *shader, *mesh);

	return id;
}

TextureID D3D11RendererManager::createTextureFromFile(std::string filePath)
{
	// ID�̎擾
	TextureID id = hash::crc32string(filePath.c_str());

	// ���ɐ����ς�
	const auto& itr = m_texturePool.find(id);
	if (m_texturePool.end() != itr) return id;

	// �V�K����
	m_texturePool[id] = std::make_unique<D3D11Texture>(m_d3dDevice.Get(), m_d3dContext.Get(), id, filePath);

	return id;
}

BatchGroupID D3D11RendererManager::creatBatchGroup(MaterialID materialID, MeshID meshID)
{
	// ID�̐���
	BatchGroupID id = materialID ^ meshID;
	//std::memcpy(&id, &materialID, sizeof(std::uint32_t));
	//std::memcpy(&id + sizeof(std::uint32_t), &meshID, sizeof(std::uint32_t));

	// ���ɂ���
	const auto& itr = m_batchGroupPool.find(id);
	if (m_batchGroupPool.end() != itr) return id;

	// �V�K����
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
