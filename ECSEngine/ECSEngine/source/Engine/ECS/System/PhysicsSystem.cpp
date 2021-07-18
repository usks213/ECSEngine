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

using namespace ecs;


 /// @brief 生成時
void PhysicsSystem::onCreate()
{
	m_graviyForce = Vector3(0, -9.8f / 60.0f, 0);
}

/// @brief 削除時
void PhysicsSystem::onDestroy()
{

}

/// @brief 更新
void PhysicsSystem::onUpdate()
{
	foreach<Transform>(
		[this](Transform& transform, Physics& phisics)
		{
			//===== 回転 =====

			// 角速度
			phisics.angularVelocity = Quaternion();
			// トルク加算
			phisics.angularVelocity *= phisics.torque;
			// 回転の更新
			transform.rotation *= phisics.angularVelocity;
			// 抵抗


			//===== 移動 =====

			// 重力
			if (phisics.gravity)
			{
				phisics.force += m_graviyForce;
			}
			// 抵抗
			phisics.force *= (Vector3(1, 1, 1) - phisics.drag);
			if (phisics.force.Length() < 0.01f)
				phisics.force = Vector3();

			// 速度更新
			phisics.velocity += phisics.force;
			// 位置の更新
			transform.translation += phisics.velocity;

		});
}


