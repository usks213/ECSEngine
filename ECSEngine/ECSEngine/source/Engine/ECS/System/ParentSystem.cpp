/*****************************************************************//**
 * \file   ParentSystem.h
 * \brief  ペアレントフォームシステム
 *
 * \author USAMI KOSHI
 * \date   2021/07/12
 *********************************************************************/

#include "ParentSystem.h"
#include <Engine/ECS/ComponentData/TransformComponentData.h>

using namespace ecs;


 /// @brief 生成時
void ParentSystem::onCreate()
{

}

/// @brief 削除時
void ParentSystem::onDestroy()
{

}

/// @brief 更新
void ParentSystem::onUpdate()
{
	foreach<LocalToWorld, LocalToParent>(
		[this](LocalToWorld& mtxWorld, LocalToParent& parent)
		{
			auto* mtxParent = getEntityManager()->getComponentData<LocalToWorld>(parent.parent);
			mtxWorld.value *= mtxParent->value;
		});
}


