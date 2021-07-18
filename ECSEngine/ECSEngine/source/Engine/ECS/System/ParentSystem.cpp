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
		transform->localToParent = Matrix();
		transform->parentScale = Vector3(1, 1, 1);

		for (auto child : getGameObjectManager()->GetChilds(id))
		{
			updateChild(child, transform->localToWorld, transform->scale);
		}
	}
}

void ParentSystem::updateChild(const GameObjectID& parent, const Matrix& localToParent, const Vector3& parentScale)
{
	auto* transform = getGameObjectManager()->getComponentData<Transform>(parent);
	transform->localToParent = localToParent;
	transform->parentScale = parentScale;

	for (auto child : getGameObjectManager()->GetChilds(parent))
	{
		updateChild(child, transform->localToWorld * localToParent, transform->scale * transform->parentScale);
	}
}
