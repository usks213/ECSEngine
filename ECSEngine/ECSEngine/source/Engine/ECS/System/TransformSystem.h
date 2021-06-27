/*****************************************************************//**
 * \file   TransformSystem.h
 * \brief  トランスフォームシステム
 * 
 * \author USAMI KOSHI
 * \date   2021/06/27
 *********************************************************************/
#pragma once

#include <Engine/ECS/Base/SystemBase.h>

namespace ecs {

	class TransformSystem final : public SystemBase
	{
	public:
		/// @brief コンストラクタ
		/// @param pWorld ワールド
		explicit TransformSystem(World* pWorld):
			SystemBase(pWorld)
		{}
		/// デストラクタ
		~TransformSystem() = default;

		/// @brief 生成時
		void onCreate() override;
		/// @brief 削除時
		void onDestroy() override;
		/// @brief 更新
		void onUpdate() override;

	private:

	};
}