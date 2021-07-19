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
#include <Engine/ECS/ComponentData/BasicComponentData.h>

using namespace ecs;


 /// @brief ������
void PhysicsSystem::onCreate()
{
	m_graviyAcceleration = Vector3(0, -0.8f, 0);
}

/// @brief �폜��
void PhysicsSystem::onDestroy()
{

}

/// @brief �X�V
void PhysicsSystem::onUpdate()
{
	// �f���^�^�C��
	float delta = (1.0f / 60.0f);

	foreach<Transform, Physics, DynamicType>(
		[this, delta](Transform& transform, Physics& physics, DynamicType& type)
		{
			//===== ��] =====

			// �p���x
			physics.angularVelocity = Quaternion();
			// �g���N���Z
			physics.angularVelocity *= physics.torque;
			// ��]�̍X�V
			transform.rotation *= physics.angularVelocity;
			// ��R


			//===== �ړ� =====
			// �d��
			if (physics.gravity)
			{
				physics.force += m_graviyAcceleration * physics.mass;
			}
			// ��R
			physics.force *= (Vector3(1, 1, 1) - physics.drag);
			if (physics.force.Length() < 0.01f)
				physics.force = Vector3();
			// �����x
			physics.acceleration = physics.force / physics.mass;
			// ���x�X�V
			physics.velocity += physics.acceleration * delta;
			// �ʒu�̍X�V
			transform.translation += physics.velocity * delta;

		});
}


