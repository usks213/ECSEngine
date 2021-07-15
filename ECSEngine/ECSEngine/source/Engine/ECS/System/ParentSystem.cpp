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
	foreach<LocalToWorld, LocalToParent>(
		[this](LocalToWorld& mtxWorld, LocalToParent& parent)
		{
			auto* mtxParent = getEntityManager()->getComponentData<LocalToWorld>(parent.parent);
			mtxWorld.value *= mtxParent->value;
		});

	for (const auto& id : m_pWorld->getGameObjectManager()->getRootList())
	{
		auto* mtxWorld = getGameObjectManager()->getComponentData<LocalToWorld>(id);
		if (mtxWorld == nullptr) continue;
		for (auto child : getGameObjectManager()->GetChilds(id))
		{
			updateChild(child, mtxWorld->value);
		}
	}
}

void ParentSystem::updateChild(const GameObjectID& parent, const Matrix& mtxParent)
{
	auto* mtxWorld = getGameObjectManager()->getComponentData<LocalToWorld>(parent);
	mtxWorld->value *= mtxParent;

	for (auto child : getGameObjectManager()->GetChilds(parent))
	{
		updateChild(child, mtxWorld->value);
	}
}
