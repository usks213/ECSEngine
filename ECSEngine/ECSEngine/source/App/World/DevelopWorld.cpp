/*****************************************************************//**
 * \file   DevelopWorld.cpp
 * \brief  開発用テストワールド
 * 
 * \author USAMI KOSHI
 * \date   2021/06/27
 *********************************************************************/

#include "DevelopWorld.h"
#include <Engine/Engine.h>

#include <Engine/ECS/Base/WorldManager.h>
#include <Engine/ECS/Base/EntityManager.h>
#include <Engine/ECS/ComponentData/TransformComponentData.h>
#include <Engine/ECS/ComponentData/RenderingComponentData.h>

#include <Engine/ECS/System/TransformSystem.h>
#include <Engine/ECS/System/RenderingSystem.h>

using namespace ecs;

struct VERTEX_3D
{
	Vector3 vtx;		// 頂点座標
	Vector3 nor;		// 法線ベクトル
	Vector4 diffuse;	// 拡散反射光
	Vector2 tex;		// テクスチャ座標
};

/// @brief スタート
void DevelopWorld::Start()
{
	
	auto* renderer = getWorldManager()->getEngine()->getRendererManager();

	// シェーダ読み込み
	ShaderDesc shaderDesc;
	shaderDesc.m_name = "Test2D";
	shaderDesc.m_stages = EShaderStageFlags::VS | EShaderStageFlags::PS;
	ShaderID shaderID = renderer->createShader(shaderDesc);

	// マテリアルの作成
	auto matID = renderer->createMaterial("TestMat", shaderID);

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

	// メッシュの生成
	MeshID meshID = renderer->createMesh("TestMesh");
	auto* pMesh = renderer->getMesh(meshID);

	pMesh->m_vertexCount = 4;
	for (int i = 0; i < 4; ++i)
	{
		pMesh->m_vertexData.positions.push_back(pVtx[i].vtx);
		pMesh->m_vertexData.normals.push_back(pVtx[i].nor);
		pMesh->m_vertexData.texcoord0s.push_back(pVtx[i].tex);
		pMesh->m_vertexData.colors.push_back(pVtx[i].diffuse);
	}

	// レンダーバッファの生成
	auto rdID = renderer->createRenderBuffer(shaderID, meshID);

	// アーキタイプ
	Archetype archetype = Archetype::create<Position, Rotation, Scale, WorldMatrix, RenderData>();

	// 初期化データ
	Scale scale;
	scale.value = Vector3(1, 1, 1);
	RenderData rd;
	rd.materialID = matID;
	rd.meshID = meshID;

	// オブジェクトの生成
	for (int i = 0; i < 5; ++i)
	{
		auto entity = getEntityManager()->createEntity(archetype);

		Position pos;
		pos.value.x = -5 + i*2;
		getEntityManager()->setComponentData<Position>(entity, pos);
		getEntityManager()->setComponentData<Scale>(entity, scale);
		getEntityManager()->setComponentData(entity, rd);
	}

	// システムの追加
	addSystem<TransformSystem>();
	addSystem<RenderingSystem>();
}

/// @brief エンド
void DevelopWorld::End()
{

}

