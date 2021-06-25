/*****************************************************************//**
 * \file   WorldManager.cpp
 * \brief  ワールドマネージャーのベースクラス
 *
 * \author USAMI KOSHI
 * \date   2021/06/14
 *********************************************************************/

#include "WorldManager.h"
#include <Engine/Engine.h>

#include <Engine/Renderer/D3D11/D3D11RendererManager.h>
#include <Engine/Renderer/D3D11/D3D11Shader.h>
#include <Engine/Renderer/D3D11/D3D11Material.h>
#include <Engine/Renderer/Base/Vertex.h>
#include <Engine/Utility/HashUtil.h>
#include <string>

using namespace DirectX;
struct VERTEX_3D 
{
	Vector3 vtx;		// 頂点座標
	Vector3 nor;		// 法線ベクトル
	Vector4 diffuse;	// 拡散反射光
	Vector2 tex;		// テクスチャ座標
} ;

struct VERTEX_3D_TEST
{
	Vector3 vtx;		// 頂点座標
	Vector2 tex;		// テクスチャ座標
	Vector4 diffuse;	// 拡散反射光
};

struct CBuffer
{
	XMMATRIX g_mWorld;
	XMMATRIX g_mView;
	XMMATRIX g_mProjection;
	XMMATRIX g_mTexture;
};


ComPtr<ID3D11Buffer> g_vertexBuffer;
UINT g_vertexSize;
//ComPtr<ID3D11Buffer> g_indexBuffer;
D3D11Shader* g_pShader;
D3D11Material* g_pMat;
static ID3D11Buffer* g_pConstantBuffer;		// 定数バッファ


void WorldManager::initialize()
{
	auto* renderer = static_cast<D3D11RendererManager*>(m_pEngine->getRendererManager());
	auto* device = renderer->m_d3dDevice.Get();
	auto* context = renderer->m_d3dContext.Get();

	// シェーダ読み込み
	ShaderDesc shaderDesc;
	shaderDesc.m_name = "Test2D";
	shaderDesc.m_stages = EShaderStageFlags::VS | EShaderStageFlags::PS;

	ShaderID shaderID = renderer->createShader(shaderDesc);
	g_pShader = static_cast<D3D11Shader*>(renderer->getShader(shaderID));
	//g_pShader = new D3D11Shader(device, shaderDesc);
	
	// 頂点座標の設定
	VERTEX_3D pVtx[4];
	// 頂点座標の設定
	pVtx[0].vtx = Vector3(-1.0f / 2, -1.0f / 2, 0.0f);
	pVtx[1].vtx = Vector3(-1.0f / 2, 1.0f / 2, 0.0f);
	pVtx[2].vtx = Vector3(1.0f / 2, -1.0f / 2, 0.0f);
	pVtx[3].vtx = Vector3(1.0f / 2, 1.0f / 2, 0.0f);
	// 法線の設定
	pVtx[0].nor = Vector3(0.0f, 0.0f, -1.0f);
	pVtx[1].nor = Vector3(0.0f, 0.0f, -1.0f);
	pVtx[2].nor = Vector3(0.0f, 0.0f, -1.0f);
	pVtx[3].nor = Vector3(0.0f, 0.0f, -1.0f);
	// 反射光の設定
	pVtx[0].diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[1].diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[2].diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[3].diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	// テクスチャ座標の設定
	pVtx[0].tex = Vector2(0.0f, 1.0f);
	pVtx[1].tex = Vector2(0.0f, 0.0f);
	pVtx[2].tex = Vector2(1.0f, 1.0f);
	pVtx[3].tex = Vector2(1.0f, 0.0f);

	// 頂点情報生成
	VertexData verData(*g_pShader, 4);
	for (int i = 0; i < 4; ++i)
	{
		verData.setPosition(pVtx[i].vtx, i);
		verData.setNormal(pVtx[i].nor, i);
		verData.setColor(pVtx[i].diffuse, i);
		verData.setTexCoord(pVtx[i].tex, 0, i);
	}
	std::vector<VERTEX_3D_TEST> test(4);
	std::memcpy(test.data(), verData.buffer.get(), verData.size * verData.count);

	D3D11_BUFFER_DESC vertex = {};
	vertex.Usage = D3D11_USAGE_IMMUTABLE;
	vertex.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertex.CPUAccessFlags = 0;
	vertex.ByteWidth = verData.size * verData.count;

	D3D11_SUBRESOURCE_DATA initData;
	ZeroMemory(&initData, sizeof(initData));
	initData.pSysMem = verData.buffer.get();

	g_vertexSize = verData.size;
	CHECK_FAILED(device->CreateBuffer(&vertex, &initData, g_vertexBuffer.GetAddressOf()));

	//// インデックスバッファ生成
	//int pIndexWk[4];
	//pIndexWk[0] = 0;
	//pIndexWk[1] = 1;
	//pIndexWk[2] = 2;
	//pIndexWk[3] = 3;

	//CD3D11_BUFFER_DESC ibd(4 * sizeof(int), D3D11_BIND_INDEX_BUFFER);
	//ZeroMemory(&initData, sizeof(initData));
	//initData.pSysMem = pIndexWk;
	//device->CreateBuffer(&ibd, &initData, g_indexBuffer.GetAddressOf());

	// マテリアルの作成
	MaterialID matID = renderer->createMaterial("TestMat", shaderID);
	g_pMat = static_cast<D3D11Material*>(renderer->getMaterial(matID));
	//g_pMat = new D3D11Material(device, 0, "TestMat", *g_pShader);

	// コンスタントバッファ
	// 拡縮
	XMMATRIX mWorld = XMMatrixScaling(10, 10, 10);
	// 回転
	mWorld *= XMMatrixRotationRollPitchYaw(XMConvertToRadians(0),
		XMConvertToRadians(0), XMConvertToRadians(0));
	// 移動
	mWorld *= XMMatrixTranslation(0, 0, 0);

	XMMATRIX mtxView, mtxProj, mtxTex;
	mtxView = XMMatrixLookAtLH(XMVectorSet(0.0f, 0.0f, -10.0f, 1.0f),
		XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f), XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
	mtxProj = XMMatrixOrthographicLH(m_pEngine->getWindowWidth(),
		m_pEngine->getWindowHeight(), 1.0f, 100.0f);
	mtxTex = XMMatrixIdentity();
	CBuffer cb;
	cb.g_mWorld = XMMatrixTranspose(mWorld);
	cb.g_mView = XMMatrixTranspose(mtxView);
	cb.g_mProjection = XMMatrixTranspose(mtxProj);
	cb.g_mTexture = XMMatrixTranspose(mtxTex);
	// cbuffer初期化
	std::memcpy(g_pMat->m_cbufferData[0][0].data.get(), &cb, sizeof(cb));
	context->UpdateSubresource(g_pMat->m_d3dCbuffer[0][0].Get(), 0, nullptr, &cb, 0, 0);


	// 定数バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(CBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	device->CreateBuffer(&bd, nullptr, &g_pConstantBuffer);

}

void WorldManager::finalize()
{

}

void WorldManager::fixedUpdate()
{

}

void WorldManager::update()
{

}

void WorldManager::render()
{
	auto* renderer = static_cast<D3D11RendererManager*>(m_pEngine->getRendererManager());
	auto* context = renderer->m_d3dContext.Get();

	// プリミティブ指定
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	// ブレンドステート
	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	context->OMSetBlendState(renderer->m_blendStates[(size_t)EBlendState::ALPHA].Get(), 
		blendFactor, 0xffffffff);
	// ラスタライザステート
	context->RSSetState(renderer->m_rasterizeStates[(size_t)ERasterizeState::CULL_NONE].Get());
	// 深度ステンシルステート
	context->OMSetDepthStencilState(renderer->m_depthStencilStates[
		(size_t)EDepthStencilState::DISABLE_TEST_AND_DISABLE_WRITE].Get(), 0);

	// シェーダー
	context->VSSetShader(g_pShader->vs, nullptr, 0);
	context->PSSetShader(g_pShader->ps, nullptr, 0);
	context->IASetInputLayout(g_pShader->m_inputLayout.Get());

	// 頂点バッファをセット
	UINT stride = g_vertexSize;
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, g_vertexBuffer.GetAddressOf(), &stride, &offset);
	// インデックスバッファをセット
	//context->IASetIndexBuffer(g_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	// コンスタントバッファ
	// 拡縮
	XMMATRIX mWorld = XMMatrixScaling(1,1,1);
	// 回転
	static float rad;
	rad += 1.0f;
	mWorld *= XMMatrixRotationRollPitchYaw(XMConvertToRadians(0),
		XMConvertToRadians(rad), XMConvertToRadians(0));
	// 移動
	mWorld *= XMMatrixTranslation(0,0,0);

	XMMATRIX mtxView, mtxProj, mtxTex;
	mtxView = XMMatrixLookAtLH(XMVectorSet(0.0f, 0.0f, -10.0f, 1.0f),
		XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f), XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
	float asoect = (float)m_pEngine->getWindowWidth() / m_pEngine->getWindowHeight();
	mtxProj = XMMatrixPerspectiveFovLH(
		XMConvertToRadians(45), asoect, 1.0f, 1000.0f);

	mtxTex = XMMatrixIdentity();
	CBuffer cb;
	cb.g_mWorld = XMMatrixTranspose(mWorld);
	cb.g_mView = XMMatrixTranspose(mtxView);
	cb.g_mProjection = XMMatrixTranspose(mtxProj);
	cb.g_mTexture = XMMatrixTranspose(mtxTex);

	//context->UpdateSubresource(g_pConstantBuffer, 0, nullptr, &cb, 0, 0);
	context->UpdateSubresource(g_pMat->m_d3dCbuffer[0][0].Get(), 0, nullptr,&cb, 0, 0);

	//context->VSSetConstantBuffers(0, 1, &g_pConstantBuffer);
	context->VSSetConstantBuffers(0, 1, g_pMat->m_d3dCbuffer[0][0].GetAddressOf());


	// ポリゴンの描画
	context->Draw(4, 0);
	//context->DrawIndexed(4, 0, 0);
}


