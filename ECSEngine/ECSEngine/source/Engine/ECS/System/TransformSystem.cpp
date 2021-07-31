/*****************************************************************//**
 * \file   TransformSystem.h
 * \brief  トランスフォームシステム
 *
 * \author USAMI KOSHI
 * \date   2021/06/27
 *********************************************************************/

#include "TransformSystem.h"
#include <Engine/ECS/ComponentData/TransformComponentData.h>
#include <Engine/ECS/ComponentData/BasicComponentData.h>

using namespace ecs;


 /// @brief 生成時
void TransformSystem::onCreate()
{
	foreach<Transform>(
		[](Transform& transform)
		{
			updateTransform(transform);
		});

	// 階層構造更新
	auto* mgr = getGameObjectManager();
	updateHierarchy(mgr, m_pWorld->getGameObjectManager()->getRootList());
}

/// @brief 削除時
void TransformSystem::onDestroy()
{

}

/// @brief 更新
void TransformSystem::onUpdate()
{
	// トランスフォーム更新
	foreach<Transform, DynamicType>(
		[](Transform& transform, DynamicType& type)
		{
			updateTransform(transform);
		});

	// 階層構造更新
	auto* mgr = getGameObjectManager();
	updateHierarchy(mgr, m_pWorld->getGameObjectManager()->getRootList());
}

inline void TransformSystem::updateTransform(Transform& transform)
{
	// 拡縮
	transform.localMatrix = Matrix::CreateScale(transform.scale);
	// 回転
	transform.localMatrix *= Matrix::CreateFromQuaternion(transform.rotation);
	// 移動
	transform.localMatrix *= Matrix::CreateTranslation(transform.translation);
}

inline void TransformSystem::updateHierarchy(GameObjectManager* mgr, std::vector<GameObjectID>& rootList)
{
	for (const auto& id : rootList)
	{
		auto* transform = mgr->getComponentData<Transform>(id);
		if (transform == nullptr) continue;
		transform->globalMatrix = transform->localMatrix;
		transform->globalScale = transform->scale;

		for (auto child : mgr->GetChilds(id))
		{
			updateChildsTransform(mgr, child, transform->globalMatrix, transform->globalScale);
		}
	}
}

inline void TransformSystem::updateChildsTransform(GameObjectManager* mgr, const GameObjectID& parent, const Matrix& globalMatrix, const Vector3& globalScale)
{
	auto* transform = mgr->getComponentData<Transform>(parent);
	transform->globalMatrix = transform->localMatrix * globalMatrix;
	transform->globalScale = transform->scale * globalScale;

	for (auto child : mgr->GetChilds(parent))
	{
		updateChildsTransform(mgr, child, transform->globalMatrix, transform->globalScale);
	}
}