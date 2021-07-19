/*****************************************************************//**
 * \file   CollisionSystem.h
 * \brief  当たり判定システム
 * 
 * \author USAMI KOSHI
 * \date   2021/07/18
 *********************************************************************/
#pragma once

#include <Engine/ECS/Base/SystemBase.h>
#include <Engine/ECS/ComponentData/TransformComponentData.h>
#include <Engine/ECS/ComponentData/PhysicsComponentData.h>

namespace ecs {

	class CollisionSystem final : public SystemBase
	{
		ECS_DECLARE_SYSTEM(CollisionSystem);
	public:
		/// @brief コンストラクタ
		/// @param pWorld ワールド
		explicit CollisionSystem(World* pWorld):
			SystemBase(pWorld)
		{}
		/// デストラクタ
		~CollisionSystem() = default;

		/// @brief 生成時
		void onCreate() override;
		/// @brief 削除時
		void onDestroy() override;
		/// @brief 更新
		void onUpdate() override;

	private:
		void Collision(const Entity& main, const std::vector<Entity>& m_subList,
			bool mainStatic, bool subStatic);

		
		bool AABBvsAABB(Transform& transform1, Physics& physics1,
			Transform& transform2, Physics& physics2, bool static2);
		bool AABBvsSphere(Transform& transform1, Physics& physics1,
			Transform& transform2, Physics& physics2, bool static2);
		bool SphereVsAABB(Transform& transform1, Physics& physics1,
			Transform& transform2, Physics& physics2, bool static2);
		bool SphereVsSphere(Transform& transform1, Physics& physics1,
			Transform& transform2, Physics& physics2, bool static2);
		bool OBBvsOBB(Transform& transform1, Physics& physics1,
			Transform& transform2, Physics& physics2, bool static2);
		bool OBBvsSphere(Transform& transform1, Physics& physics1,
			Transform& transform2, Physics& physics2, bool static2);
		bool SphereVsOBB(Transform& transform1, Physics& physics1,
			Transform& transform2, Physics& physics2, bool static2);
	};
}