/*****************************************************************//**
 * \file   CollisionSystem.h
 * \brief  �����蔻��V�X�e��
 *
 * \author USAMI KOSHI
 * \date   2021/07/18
 *********************************************************************/

#include "CollisionSystem.h"
#include <Engine/ECS/ComponentData/TransformComponentData.h>
#include <Engine/ECS/ComponentData/PhysicsComponentData.h>
#include <Engine/ECS/System/QuadTreeSystem.h>


using namespace ecs;


 /// @brief ������
void CollisionSystem::onCreate()
{

}

/// @brief �폜��
void CollisionSystem::onDestroy()
{

}

/// @brief �X�V
void CollisionSystem::onUpdate()
{
	// �l���؃V�X�e���̎擾
	auto* quadTree = m_pWorld->getSystem<QuadTreeSystem>();
	if (quadTree == nullptr) return;

	// ���I�I�u�W�F�N�g�ƐÓI�I�u�W�F�N�g�𕪂��ē����蔻��

	//--- ���I�I�u�W�F�N�g���m
	// ���C����Ԃ̓����蔻��
	for (int i = 0; i < quadTree->getCellNum(); ++i)
	{
		for (auto& entity : quadTree->m_dynamicMainList[i])
		{
			Collision(entity, quadTree->m_dynamicMainList[i]);
		}
	}
	// �T�u��Ԃ̓����蔻��
	for (int i = 0; i < quadTree->getCellNum(); ++i)
	{
		for (auto& entity : quadTree->m_dynamicMainList[i])
		{
			Collision(entity, quadTree->m_dynamicSubList[i]);
		}
	}

	//--- ���I�I�u�W�F�N�g�ƐÓI�I�u�W�F�N�g
	// ���C����Ԃ̓����蔻��
	for (int i = 0; i < quadTree->getCellNum(); ++i)
	{
		for (auto& entity : quadTree->m_dynamicMainList[i])
		{
			Collision(entity, quadTree->m_staticMainList[i]);
		}
	}
	// �T�u��Ԃ̓����蔻��
	for (int i = 0; i < quadTree->getCellNum(); ++i)
	{
		for (auto& entity : quadTree->m_dynamicMainList[i])
		{
			Collision(entity, quadTree->m_staticSubList[i]);
		}
	}
	//--- �ÓI�I�u�W�F�N�g�Ɠ��I�I�u�W�F�N�g
		// �T�u��Ԃ̓����蔻��
	for (int i = 0; i < quadTree->getCellNum(); ++i)
	{
		for (auto& entity : quadTree->m_staticMainList[i])
		{
			Collision(entity, quadTree->m_dynamicSubList[i]);
		}
	}
}


void CollisionSystem::Collision(const Entity& main, const std::vector<Entity>& m_subList)
{
	Vector3 min(-0.5f, -0.5f, -0.5f);
	Vector3 max(0.5f, 0.5f, 0.5f);

	// �T�u���[�v
	for (const auto& sub : m_subList)
	{
		// ����������
		if (main.m_chunkIndex == sub.m_chunkIndex &&
			main.m_index == sub.m_index) continue;

		//--- �����蔻�菈�� ---
		auto* mainTrans = getEntityManager()->getComponentData<Transform>(main);
		auto* subTrans = getEntityManager()->getComponentData<Transform>(sub);

		Vector3  min1 = Vector3::Transform(min, mainTrans->globalMatrix);
		Vector3  max1 = Vector3::Transform(max, mainTrans->globalMatrix);
		Vector3  min2 = Vector3::Transform(min, subTrans->globalMatrix);
		Vector3  max2 = Vector3::Transform(max, subTrans->globalMatrix);

		// AABB
		if (min1.x < max2.x && min2.x < max1.x &&
			min1.y < max2.y && min2.y < max1.y &&
			min1.z < max2.z && min2.z < max1.z)
		{
			auto* mainPhysics = getEntityManager()->getComponentData<Physics>(main);
			auto* subPhysics = getEntityManager()->getComponentData<Physics>(sub);
		}
	}
}