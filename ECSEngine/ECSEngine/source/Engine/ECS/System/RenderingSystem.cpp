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
	auto* renderer = m_pWorld->getWorldManager()->getEngine()->getRendererManager();

	foreach<RenderData, WorldMatrix>(
		[&renderer](RenderData& rd, WorldMatrix& mtxWorld)
		{
			renderer->render(mtxWorld.value, rd.materialID, rd.meshID);
		});
}


