/*****************************************************************//**
 * \file   RenderingSystem.h
 * \brief  �����_�����O�V�X�e��
 *
 * \author USAMI KOSHI
 * \date   2021/06/27
 *********************************************************************/
#pragma once

#include <Engine/ECS/Base/SystemBase.h>

namespace ecs {

	class RenderingSystem final : public SystemBase
	{
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

	private:

	};
}