/*****************************************************************//**
 * \file   RenderingSystem.h
 * \brief  �����_�����O�V�X�e��
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
		/// @brief �R���X�g���N�^
		/// @param pWorld ���[���h
		explicit RenderingSystem(World* pWorld) :
			SystemBase(pWorld)
		{}
		/// �f�X�g���N�^
		~RenderingSystem() = default;

		/// @brief ������
		void onCreate() override;
		/// @brief �폜��
		void onDestroy() override;
		/// @brief �X�V
		void onUpdate() override;


		static inline void updateTransform(Transform& transform);
		static inline void updateCamera(Camera& camera, Transform& transform, float width, float height);

	private:

	};
}