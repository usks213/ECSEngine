/*****************************************************************//**
 * \file   PhysicsSystem.h
 * \brief  物理システム
 * 
 * \author USAMI KOSHI
 * \date   2021/07/18
 *********************************************************************/
#pragma once

#include <Engine/ECS/Base/SystemBase.h>
#include <Engine/Utility/Mathf.h>


namespace ecs {

	class PhysicsSystem final : public SystemBase
	{
		ECS_DECLARE_SYSTEM(PhysicsSystem);
	public:
		/// @brief コンストラクタ
		/// @param pWorld ワールド
		explicit PhysicsSystem(World* pWorld):
			SystemBase(pWorld)
		{}
		/// デストラクタ
		~PhysicsSystem() = default;

		/// @brief 生成時
		void onCreate() override;
		/// @brief 削除時
		void onDestroy() override;
		/// @brief 更新
		void onUpdate() override;

	private:
		/// @brief 重力
		Vector3 m_graviyAcceleration;
	};
}