/*****************************************************************//**
 * \file   AnimationSystem.h
 * \brief  アニメーションシステム
 * 
 * \author USAMI KOSHI
 * \date   2021/09/30
 *********************************************************************/
#pragma once

#include <Engine/ECS/Base/SystemBase.h>
#include <Engine/ECS/Base/GameObjectManager.h>


namespace ecs {

	class AnimationSystem final : public SystemBase
	{
		ECS_DECLARE_SYSTEM(AnimationSystem);
	public:
		/// @brief コンストラクタ
		/// @param pWorld ワールド
		explicit AnimationSystem(World* pWorld):
			SystemBase(pWorld)
		{}
		/// デストラクタ
		~AnimationSystem() = default;

		/// @brief 生成時
		void onCreate() override;
		/// @brief 削除時
		void onDestroy() override;
		/// @brief 更新
		void onUpdate() override;

		/// @brief ゲームオブジェクト更新前に一度だけ呼ばれるコールバック
		void onStartGameObject(const GameObjectID& id) override;

	private:

	};
}