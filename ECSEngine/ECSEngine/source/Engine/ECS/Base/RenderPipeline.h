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
#include <Engine/ECS/Base/GameObject.h>
#include <Engine/ECS/ComponentData/TransformComponentData.h>
#include <Engine/ECS/ComponentData/RenderingComponentData.h>
#include <Engine/ECS/ComponentData/CameraComponentData.h>
#include <Engine/ECS/ComponentData/LightComponentData.h>


namespace ecs {

	class RenderPipeline final : public SystemBase
	{
		ECS_DECLARE_SYSTEM(RenderPipeline);

	public:
		struct PipelineData
		{
			Camera*				camera = nullptr;
			DirectionalLight*	directionalLight = nullptr;
		};
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

		
		void cullingPass(RendererManager* renderer, const PipelineData& data);

		void beginPass(RendererManager* renderer, const PipelineData& data);
		void prePass(RendererManager* renderer, const PipelineData& data);
		void gbufferPass(RendererManager* renderer, const PipelineData& data);
		void shadowPass(RendererManager* renderer, const PipelineData& data);
		void opaquePass(RendererManager* renderer, const PipelineData& data);
		void skyPass(RendererManager* renderer, const PipelineData& data);
		void transparentPass(RendererManager* renderer, const PipelineData& data);
		void postPass(RendererManager* renderer, const PipelineData& data);
		void endPass(RendererManager* renderer, const PipelineData& data);

		PipelineData GetPipelineData();

		static inline void updateCamera(Camera& camera, Transform& transform, float width, float height);
		static void updateBoneMatrix(GameObjectManager* goMgr, const GameObjectID& goID, Mesh& mesh);

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
			bool			isBone = false;
			MeshID			meshID;
			GameObjectID	rootID;
		};
		std::vector<RenderingData> m_opequeList;
		std::vector<RenderingData> m_transparentList;
		std::vector<RenderingData> m_deferredList;

		std::unordered_map<MeshID, std::vector<Matrix>>	m_shadowList;
		Vector2				m_shadowMapSize;

		DirectionalLightData		m_directionalLight;
		std::vector<PointLightData> m_pointLights;
		std::vector<SpotLightData>	m_spotLights;



		ShaderID			m_depthWriteShader;
		MaterialID			m_depthWriteMat;
		MaterialID			m_defferdLitMat;
		RenderBufferID		m_quadRb;
	};
}
