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

void WorldManager::initialize()
{
	auto* renderer = static_cast<D3D11RendererManager*>(m_pEngine->getRendererManager());
	auto* device = renderer->m_d3dDevice.Get();

	// シェーダ読み込み
	ShaderDesc shaderDesc;
	shaderDesc.m_name = "Test";
	shaderDesc.m_stages = EShaderStageFlags::VS | EShaderStageFlags::PS;
	D3D11Shader shader(device, shaderDesc);
	
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
	VertexData verData(shader, 8);
	for (int i = 0; i < 4; ++i)
	{
		verData.setPosition(pVtx[i].vtx, i);
		verData.setNormal(pVtx[i].nor, i);
		verData.setColor(pVtx[i].diffuse, i);
		verData.setTexCoord(pVtx[i].tex, 0, i);
	}

	D3D11_BUFFER_DESC vertex;

	//renderer->m_d3dDevice->CreateBuffer();
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

}


