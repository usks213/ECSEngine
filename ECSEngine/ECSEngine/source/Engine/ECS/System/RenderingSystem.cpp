/*****************************************************************//**
 * \file   RenderingSystem.h
 * \brief  レンダリングシステム
 *
 * \author USAMI KOSHI
 * \date   2021/06/27
 *********************************************************************/

#include "RenderingSystem.h"
#include <Engine/ECS/ComponentData/TransformComponentData.h>
#include <Engine/ECS/ComponentData/RenderingComponentData.h>

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

	// システムバッファの設定
	XMMATRIX mtxView, mtxProj;
	mtxView = XMMatrixLookAtLH(XMVectorSet(0.0f, 10.0f, -10.0f, 1.0f),
		XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f), XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
	float asoect = (float)engine->getWindowWidth() / engine->getWindowHeight();
	mtxProj = XMMatrixPerspectiveFovLH(
		XMConvertToRadians(45), asoect, 1.0f, 1000.0f);
	D3D::SystemBuffer sysBuffer;
	sysBuffer._mView = XMMatrixTranspose(mtxView);
	sysBuffer._mProj = XMMatrixTranspose(mtxProj);
	renderer->setD3DSystemBuffer(sysBuffer);


	// オブジェクト描画
	foreach<RenderData, WorldMatrix>(
		[&renderer](RenderData& rd, WorldMatrix& mtxWorld)
		{
			renderer->setD3D11Material(rd.materialID);

			renderer->setD3DTransformBuffer(mtxWorld.value);

			renderer->render(rd.materialID, rd.meshID);
		});
}


