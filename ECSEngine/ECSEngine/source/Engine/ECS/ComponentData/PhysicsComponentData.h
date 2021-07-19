/*****************************************************************//**
 * \file   PhysicsComponentData.h
 * \brief  物理コンポーネントデータ
 * 
 * \author USAMI KOSHI
 * \date   2021/07/18
 *********************************************************************/
#pragma once

#include <Engine/ECS/Base/IComponentData.h>
#include <Engine/Utility/Mathf.h>


namespace ecs {

	/// @brief 物理コンポーネントデータ
	struct Physics : IComponentData
	{
		ECS_DECLARE_COMPONENT_DATA(Physics);

		/// @brief コライダーの種類
		enum class ColliderType : std::uint32_t
		{
			AABB,
			Sphere,
			OBB,
			Max,
		};

		/// @brief コンストラクタ
		/// @param collider コライダー種別
		/// @param trigger 押し出しなし
		/// @param gravity 重力使用
		explicit Physics(ColliderType collider, bool trigger = false, bool gravity = true) :
			colliderType(collider), trigger(trigger), 
			gravity(gravity), mass(2.0f), drag(),
			e(1.0f), staticFriction(1.0f), dynamicFriction(1.0f)
		{
		}

		//--- 物理パラメータ

		/// @brief コライダータイプ
		ColliderType colliderType;
		/// @brief 押し出し使用
		bool trigger;

		/// @brief 重力使用
		bool gravity;
		/// @brief 質量
		float mass;
		/// @brief 抵抗
		Vector3 drag;
		/// @brief 慣性テンソル?
		// Quaternion m_

		/// @brief 反発係数
		float e;
		/// @brief 静摩擦係数
		float staticFriction;
		/// @brief 動摩擦係数
		float dynamicFriction;

		//--- 内部処理パラメータ
		/// @brief 速度
		Vector3 velocity;
		/// @brief 加速度
		Vector3 acceleration;
		/// @brief 力
		Vector3 force;
		/// @brief 角速度
		Quaternion angularVelocity;
		/// @brief モーメント
		Quaternion torque;
	};
}