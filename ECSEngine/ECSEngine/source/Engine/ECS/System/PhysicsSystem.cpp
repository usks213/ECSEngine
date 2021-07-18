/*****************************************************************//**
 * \file   PhysicsSystem.h
 * \brief  �����V�X�e��
 *
 * \author USAMI KOSHI
 * \date   2021/07/18
 *********************************************************************/

#include "PhysicsSystem.h"
#include <Engine/ECS/ComponentData/TransformComponentData.h>
#include <Engine/ECS/ComponentData/PhysicsComponentData.h>

using namespace ecs;


 /// @brief ������
void PhysicsSystem::onCreate()
{
	m_graviyForce = Vector3(0, -9.8f / 60.0f, 0);
}

/// @brief �폜��
void PhysicsSystem::onDestroy()
{

}

/// @brief �X�V
void PhysicsSystem::onUpdate()
{
	foreach<Transform>(
		[this](Transform& transform, Physics& phisics)
		{
			//===== ��] =====

			// �p���x
			phisics.angularVelocity = Quaternion();
			// �g���N���Z
			phisics.angularVelocity *= phisics.torque;
			// ��]�̍X�V
			transform.rotation *= phisics.angularVelocity;
			// ��R


			//===== �ړ� =====

			// �d��
			if (phisics.gravity)
			{
				phisics.force += m_graviyForce;
			}
			// ��R
			phisics.force *= (Vector3(1, 1, 1) - phisics.drag);
			if (phisics.force.Length() < 0.01f)
				phisics.force = Vector3();

			// ���x�X�V
			phisics.velocity += phisics.force;
			// �ʒu�̍X�V
			transform.translation += phisics.velocity;

		});
}


