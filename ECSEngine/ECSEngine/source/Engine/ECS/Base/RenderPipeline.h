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
			SystemBase(pWorld),
			m_renderTarget(NONE_RENDER_TARGET_ID)
		{
			onCreate();
		}
		/// デストラクタ
		~RenderPipeline()
		{
			onDestroy();
		}

		/// @brief 生成時
		void onCreate() override;
		/// @brief 削除時
		void onDestroy() override;
		/// @brief 更新
		void onUpdate() override;

		
		void cullingPass(Camera& camera);
		void beginPass(Camera& camera);
		void prePass(Camera& camera);
		void gbufferPass(Camera& camera);
		void shadowPass(Camera& camera);
		void opaquePass(Camera& camera);
		void skyPass(Camera& camera);
		void transparentPass(Camera& camera);
		void postPass(Camera& camera);
		void endPass(Camera& camera);

		static inline void updateCamera(Camera& camera, Transform& transform, float width, float height);

	public:
		RenderTargetID m_renderTarget;

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
		std::vector<RenderingData> m_deferredList;

		std::vector<Matrix>		m_shadowList;

		DirectionalLightData		m_directionalLight;
		std::vector<PointLightData> m_pointLights;
		std::vector<SpotLightData>	m_spotLights;

		MaterialID			m_defferdLitMat;
		RenderBufferID		m_quadRb;
	};
}
