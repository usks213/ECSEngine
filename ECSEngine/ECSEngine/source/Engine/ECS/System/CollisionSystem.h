/*****************************************************************//**
 * \file   CollisionSystem.h
 * \brief  当たり判定システム
 * 
 * \author USAMI KOSHI
 * \date   2021/07/18
 *********************************************************************/
#pragma once

#include <Engine/ECS/Base/SystemBase.h>

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
		void Collision(const Entity& main, const std::vector<Entity>& m_subList);
	};
}