/*****************************************************************//**
 * \file   ParentSystem.h
 * \brief  �y�A�����g�t�H�[���V�X�e��
 *
 * \author USAMI KOSHI
 * \date   2021/07/12
 *********************************************************************/

#include "ParentSystem.h"
#include <Engine/ECS/ComponentData/TransformComponentData.h>

using namespace ecs;


 /// @brief ������
void ParentSystem::onCreate()
{

}

/// @brief �폜��
void ParentSystem::onDestroy()
{

}

/// @brief �X�V
void ParentSystem::onUpdate()
{
	for (const auto& id : m_pWorld->getGameObjectManager()->getRootList())
	{
		auto* transform = getGameObjectManager()->getComponentData<Transform>(id);
		if (transform == nullptr) continue;
		for (auto child : getGameObjectManager()->GetChilds(id))
		{
			transform->localToParent = Matrix();
			transform->parentScale = Vector3(1, 1, 1);
			updateChild(child, transform->localToWorld, transform->localScale);
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
		updateChild(child, transform->localToWorld * localToParent, transform->localScale * transform->parentScale);
	}
}
