/*****************************************************************//**
 * \file   PhysicsSystem.h
 * \brief  物理システム
 *
 * \author USAMI KOSHI
 * \date   2021/07/18
 *********************************************************************/

#include "PhysicsSystem.h"
#include <Engine/ECS/ComponentData/TransformComponentData.h>
#include <Engine/ECS/ComponentData/PhysicsComponentData.h>
#include <Engine/ECS/ComponentData/BasicComponentData.h>

using namespace ecs;


 /// @brief 生成時
void PhysicsSystem::onCreate()
{
	m_graviyAcceleration = Vector3(0, -0.8f, 0);
}

/// @brief 削除時
void PhysicsSystem::onDestroy()
{

}

/// @brief 更新
void PhysicsSystem::onUpdate()
{
	// デルタタイム
	float delta = (1.0f / 60.0f);

	foreach<Transform, Physics, DynamicType>(
		[this, delta](Transform& transform, Physics& physics, DynamicType& type)
		{
			//===== 回転 =====

			// 角速度
			physics.angularVelocity = Quaternion();
			// トルク加算
			physics.angularVelocity *= physics.torque;
			// 回転の更新
			transform.rotation *= physics.angularVelocity;
			// 抵抗


			//===== 移動 =====
			// 重力
			if (physics.gravity)
			{
				physics.force += m_graviyAcceleration * physics.mass;
			}
			// 抵抗
			physics.force *= (Vector3(1, 1, 1) - physics.drag);
			if (physics.force.Length() < 0.01f)
				physics.force = Vector3();
			// 加速度
			physics.acceleration = physics.force / physics.mass;
			// 速度更新
			physics.velocity += physics.acceleration * delta;
			// 位置の更新
			transform.translation += physics.velocity * delta;

		});
}


