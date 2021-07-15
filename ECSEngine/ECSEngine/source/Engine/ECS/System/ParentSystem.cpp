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
	for (const auto& id : m_pWorld->getGameObjectManager()->getRootList())
	{
		auto* transform = getGameObjectManager()->getComponentData<Transform>(id);
		if (transform == nullptr) continue;
		for (auto child : getGameObjectManager()->GetChilds(id))
		{
			updateChild(child, transform->LocalToWorld);
		}
	}
}

void ParentSystem::updateChild(const GameObjectID& parent, const Matrix& mtxParent)
{
	auto* transform = getGameObjectManager()->getComponentData<Transform>(parent);
	transform->LocalToParent = mtxParent;

	for (auto child : getGameObjectManager()->GetChilds(parent))
	{
		updateChild(child, transform->LocalToWorld * transform->LocalToParent);
	}
}
