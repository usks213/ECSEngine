/*****************************************************************//**
 * \file   RenderPipeline.h
 * \brief  レンダーパイプライン
 * 
 * \author USAMI KOSHI
 * \date   2021/07/30
 *********************************************************************/
#pragma once

#include <Engine/ECS/Base/SystemBase.h>
#include <Engine/Engine.h>
#include <Engine/ECS/ComponentData/TransformComponentData.h>
#include <Engine/ECS/ComponentData/RenderingComponentData.h>
#include <Engine/ECS/ComponentData/CameraComponentData.h>
#include <Engine/ECS/ComponentData/LightComponentData.h>


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

		
		void cullingPass(Camera& camera);
		void beginPass(Camera& camera);
		void prePass();
		void gbufferPass();
		void shadowPass();
		void opaquePass();
		void skyPass();
		void transparentPass();
		void postPass();
		void endPass(Camera& camera);

		static inline void updateCamera(Camera& camera, Transform& transform, float width, float height);

	private:

		std::unordered_map<BatchGroupID, std::vector<Matrix>> m_batchList;
		struct RenderingData
		{
			MaterialID		materialID;
			RenderBufferID	renderBufferID;
			Matrix			worldMatrix;
			float			cameraLengthSqr;
		};
		std::vector<RenderingData> m_opequeList;
		std::vector<RenderingData> m_transparentList;

		std::vector<Matrix>		m_shadowList;

		DirectionalLightData		m_directionalLight;
		std::vector<PointLightData> m_pointLights;
		std::vector<SpotLightData>	m_spotLights;
	};
}
