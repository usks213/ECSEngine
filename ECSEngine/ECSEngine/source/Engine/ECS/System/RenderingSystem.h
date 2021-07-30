/*****************************************************************//**
 * \file   RenderingSystem.h
 * \brief  レンダリングシステム
 *
 * \author USAMI KOSHI
 * \date   2021/06/27
 *********************************************************************/
#pragma once

#include <Engine/ECS/Base/SystemBase.h>

#include <Engine/ECS/ComponentData/TransformComponentData.h>
#include <Engine/ECS/ComponentData/RenderingComponentData.h>
#include <Engine/ECS/ComponentData/CameraComponentData.h>

namespace ecs {

	class RenderingSystem final : public SystemBase
	{
		ECS_DECLARE_SYSTEM(RenderingSystem);
	public:
		/// @brief コンストラクタ
		/// @param pWorld ワールド
		explicit RenderingSystem(World* pWorld) :
			SystemBase(pWorld)
		{}
		/// デストラクタ
		~RenderingSystem() = default;

		/// @brief 生成時
		void onCreate() override;
		/// @brief 削除時
		void onDestroy() override;
		/// @brief 更新
		void onUpdate() override;


		static inline void updateTransform(Transform& transform);
		static inline void updateCamera(Camera& camera, Transform& transform, float width, float height);

	private:

	};
}