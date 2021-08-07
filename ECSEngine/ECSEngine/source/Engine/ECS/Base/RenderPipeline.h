/*****************************************************************//**
 * \file   RenderPipeline.h
 * \brief  �����_�[�p�C�v���C��
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
		/// @brief �R���X�g���N�^
		/// @param pWorld ���[���h
		explicit RenderPipeline(World* pWorld) :
			SystemBase(pWorld)
		{}
		/// �f�X�g���N�^
		~RenderPipeline() = default;

		/// @brief ������
		void onCreate() override;
		/// @brief �폜��
		void onDestroy() override;
		/// @brief �X�V
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
