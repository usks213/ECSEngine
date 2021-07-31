/*****************************************************************//**
 * \file   TransformSystem.h
 * \brief  �g�����X�t�H�[���V�X�e��
 *
 * \author USAMI KOSHI
 * \date   2021/06/27
 *********************************************************************/

#include "TransformSystem.h"
#include <Engine/ECS/ComponentData/TransformComponentData.h>
#include <Engine/ECS/ComponentData/BasicComponentData.h>

using namespace ecs;


 /// @brief ������
void TransformSystem::onCreate()
{
	foreach<Transform>(
		[](Transform& transform)
		{
			updateTransform(transform);
		});

	// �K�w�\���X�V
	auto* mgr = getGameObjectManager();
	updateHierarchy(mgr, m_pWorld->getGameObjectManager()->getRootList());
}

/// @brief �폜��
void TransformSystem::onDestroy()
{

}

/// @brief �X�V
void TransformSystem::onUpdate()
{
	// �g�����X�t�H�[���X�V
	foreach<Transform, DynamicType>(
		[](Transform& transform, DynamicType& type)
		{
			updateTransform(transform);
		});

	// �K�w�\���X�V
	auto* mgr = getGameObjectManager();
	updateHierarchy(mgr, m_pWorld->getGameObjectManager()->getRootList());
}

inline void TransformSystem::updateTransform(Transform& transform)
{
	// �g�k
	transform.localMatrix = Matrix::CreateScale(transform.scale);
	// ��]
	transform.localMatrix *= Matrix::CreateFromQuaternion(transform.rotation);
	// �ړ�
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