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
		transform->globalMatrix = transform->localMatrix;
		transform->globalScale = transform->scale;

		for (auto child : getGameObjectManager()->GetChilds(id))
		{
			updateChild(child, transform->globalMatrix, transform->globalScale);
		}
	}
}

void ParentSystem::updateChild(const GameObjectID& parent, const Matrix& globalMatrix, const Vector3& globalScale)
{
	auto* transform = getGameObjectManager()->getComponentData<Transform>(parent);
	transform->globalMatrix = transform->localMatrix * globalMatrix;
	transform->globalScale = transform->scale * globalScale;

	for (auto child : getGameObjectManager()->GetChilds(parent))
	{
		updateChild(child, transform->globalMatrix, transform->globalScale);
	}
}
