/*****************************************************************//**
 * \file   ParentSystem.h
 * \brief  ペアレントフォームシステム
 * 
 * \author USAMI KOSHI
 * \date   2021/07/12
 *********************************************************************/
#pragma once

#include <Engine/ECS/Base/SystemBase.h>
#include <Engine/ECS/ComponentData/TransformComponentData.h>


namespace ecs {

	class ParentSystem final : public SystemBase
	{
	public:
		/// @brief コンストラクタ
		/// @param pWorld ワールド
		explicit ParentSystem(World* pWorld):
			SystemBase(pWorld)
		{}
		/// デストラクタ
		~ParentSystem() = default;

		/// @brief 生成時
		void onCreate() override;
		/// @brief 削除時
		void onDestroy() override;
		/// @brief 更新
		void onUpdate() override;

	private:
		void updateChild(const GameObjectID& parent, const Transform& parentTransform);
	};
}