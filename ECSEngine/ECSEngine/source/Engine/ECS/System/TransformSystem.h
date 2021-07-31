/*****************************************************************//**
 * \file   TransformSystem.h
 * \brief  トランスフォームシステム
 * 
 * \author USAMI KOSHI
 * \date   2021/06/27
 *********************************************************************/
#pragma once

#include <Engine/ECS/Base/SystemBase.h>
#include <Engine/ECS/Base/GameObjectManager.h>
#include <Engine/ECS/ComponentData/TransformComponentData.h>


namespace ecs {

	class TransformSystem final : public SystemBase
	{
		ECS_DECLARE_SYSTEM(TransformSystem);
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

		/// @brief ゲームオブジェクト更新前に一度だけ呼ばれるコールバック
		void onStartGameObject(const GameObjectID& id) override;

		static inline void updateTransform(Transform& transform);
		static inline void updateHierarchy(GameObjectManager* mgr, std::vector<GameObjectID>& rootList);
		static inline void updateChildsTransform(GameObjectManager* mgr, const GameObjectID& parent, const Matrix& globalMatrix, const Vector3& globalScale);

	private:

	};
}