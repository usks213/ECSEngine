/*****************************************************************//**
 * \file   RenderPipeline.h
 * \brief  レンダーパイプライン
 * 
 * \author USAMI KOSHI
 * \date   2021/07/30
 *********************************************************************/
#pragma once

#include <Engine/ECS/Base/SystemBase.h>

#include <Engine/ECS/ComponentData/TransformComponentData.h>
#include <Engine/ECS/ComponentData/RenderingComponentData.h>
#include <Engine/ECS/ComponentData/CameraComponentData.h>

namespace ecs {

	class RenderPipeline final : public SystemBase
	{
		ECS_DECLARE_SYSTEM(RenderPipeline);
	public:
		/// @brief コンストラクタ
		/// @param pWorld ワールド
		explicit RenderPipeline(World* pWorld) :
			SystemBase(pWorld)
		{}
		/// デストラクタ
		~RenderPipeline() = default;

		/// @brief 生成時
		void onCreate() override;
		/// @brief 削除時
		void onDestroy() override;
		/// @brief 更新
		void onUpdate() override;

		
		void beginPass();
		void systemPass(Camera& camera);
		void cullingPass(Camera& camera);
		void renderingPass();
		void endPass(Camera& camera);

		static inline void updateCamera(Camera& camera, Transform& transform, float width, float height);

	private:

	};
}
