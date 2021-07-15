/*****************************************************************//**
 * \file   RenderingSystem.h
 * \brief  レンダリングシステム
 *
 * \author USAMI KOSHI
 * \date   2021/06/27
 *********************************************************************/

#include "RenderingSystem.h"
#include <Engine/ECS/Base/EntityManager.h>

#include <Engine/ECS/ComponentData/TransformComponentData.h>
#include <Engine/ECS/ComponentData/RenderingComponentData.h>
#include <Engine/ECS/ComponentData/CameraComponentData.h>

#include <Engine/Engine.h>
#include <Engine/Renderer/D3D11/D3D11RendererManager.h>

using namespace ecs;


/// @brief 生成時
void RenderingSystem::onCreate()
{

}

/// @brief 削除時
void RenderingSystem::onDestroy()
{

}

/// @brief 更新
void RenderingSystem::onUpdate()
{
	// レンダラーマネージャー
	auto* engine = m_pWorld->getWorldManager()->getEngine();
	auto* renderer = static_cast<D3D11RendererManager*>(engine->getRendererManager());


	// カメラ設定
	Camera* mainCamera = nullptr;
	Vector3 cameraPos;
	foreach<Camera, LocalToWorld>(
		[&mainCamera, &cameraPos, &engine](Camera& camera, LocalToWorld& mtxWorld)
		{
			// ビューマトリックス更新
			Vector3 pos = mtxWorld.value.Translation();
			Vector3 target = pos + mtxWorld.value.Forward();
			Vector3 up = mtxWorld.value.Up();
			camera.view = Matrix::CreateLookAt(pos, target, up);

			// ウィンドウサイズ
			camera.width = engine->getWindowWidth();
			camera.height = engine->getWindowHeight();
			camera.aspect = camera.width / camera.height;

			// プロジェクションマトリックス更新
			if (camera.isOrthographic)
			{
				// 並行投影
				camera.projection = Matrix::CreateOrthographic(
					camera.width, camera.height, camera.nearZ, camera.farZ);
			}
			else
			{
				// 透視投影
				camera.projection = Matrix::CreatePerspectiveFieldOfView(
					camera.fovY, camera.aspect, camera.nearZ, camera.farZ);
			}

			// メインカメラ
			mainCamera = &camera;
			cameraPos = pos;
		});

	// カメラなし
	if (mainCamera == nullptr) return;

	// システムバッファの設定
	//XMMATRIX mtxView, mtxProj;
	//mtxView = XMMatrixLookAtLH(XMVectorSet(0.0f, 10.0f, -10.0f, 1.0f),
	//	XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f), XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
	//float asoect = (float)engine->getWindowWidth() / engine->getWindowHeight();
	//mtxProj = XMMatrixPerspectiveFovLH(
	//	XMConvertToRadians(45), asoect, 1.0f, 1000.0f);

	// ディレクショナルライト
	DirectionalLightData dirLit;
	dirLit.ambient = Vector4(0.3f, 0.3f, 0.3f, 0.3f);
	dirLit.diffuse = Vector4(2.0f, 2.0f, 2.0f, 1.0f);
	dirLit.specular = Vector4(0.1f, 0.1f, 0.1f, 0.2f);
	dirLit.direction = Vector4(1.f, -1.5f, -1.f, 0.0f);


	// システムバッファの設定
	D3D::SystemBuffer sysBuffer;
	sysBuffer._mView = mainCamera->view.Transpose();
	sysBuffer._mProj = mainCamera->projection.Transpose();
	sysBuffer._viewPos = Vector4(cameraPos);
	sysBuffer._directionalLit = dirLit;
	renderer->setD3DSystemBuffer(sysBuffer);


	// バッチ描画
	for (const auto& bitchID : renderer->m_batchGroupPool)
	{
		auto* pBitch = renderer->getBatchGroup(bitchID.first);
		const auto* mat = renderer->getMaterial(pBitch->m_materialID);
		const auto& rdID = renderer->createRenderBuffer(mat->m_shaderID, pBitch->m_meshID);

		renderer->setD3D11Material(pBitch->m_materialID);
		renderer->setD3D11RenderBuffer(rdID);

		
		for (auto* chunk : getEntityManager()->getChunkListByTag(bitchID.first))
		{
			auto mtxArray = chunk->getComponentArray<LocalToWorld>();
			for (auto i = 0u; i < mtxArray.Count(); ++i)
			{
				renderer->setD3DTransformBuffer(mtxArray[i].value);
				renderer->d3dRender(rdID);
			}
		}
	}


	// オブジェクト描画
	foreach<RenderData, LocalToWorld>(
		[&renderer](RenderData& rd, LocalToWorld& mtxWorld)
		{
			const auto* mat = renderer->getMaterial(rd.materialID);
			const auto& rdID = renderer->createRenderBuffer(mat->m_shaderID, rd.meshID);

			renderer->setD3D11Material(rd.materialID);

			renderer->setD3DTransformBuffer(mtxWorld.value);

			renderer->setD3D11RenderBuffer(rdID);

			renderer->d3dRender(rdID);
		});
}


