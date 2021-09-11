/*****************************************************************//**
 * \file   DevelopSystem.h
 * \brief  開発用システム
 * 
 * \author USAMI KOSHI
 * \date   2021/09/01
 *********************************************************************/
#pragma once

#include <Engine/ECS/Base/SystemBase.h>
#include <Engine/ECS/Base/GameObjectManager.h>


namespace ecs {

	class DevelopSystem final : public SystemBase
	{
		ECS_DECLARE_SYSTEM(DevelopSystem);
	public:
		/// @brief コンストラクタ
		/// @param pWorld ワールド
		explicit DevelopSystem(World* pWorld):
			SystemBase(pWorld)
		{}
		/// デストラクタ
		~DevelopSystem() = default;

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