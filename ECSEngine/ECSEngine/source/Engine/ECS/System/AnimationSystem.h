/*****************************************************************//**
 * \file   AnimationSystem.h
 * \brief  �A�j���[�V�����V�X�e��
 * 
 * \author USAMI KOSHI
 * \date   2021/09/30
 *********************************************************************/
#pragma once

#include <Engine/ECS/Base/SystemBase.h>
#include <Engine/ECS/Base/GameObjectManager.h>


namespace ecs {

	class AnimationSystem final : public SystemBase
	{
		ECS_DECLARE_SYSTEM(AnimationSystem);
	public:
		/// @brief �R���X�g���N�^
		/// @param pWorld ���[���h
		explicit AnimationSystem(World* pWorld):
			SystemBase(pWorld)
		{}
		/// �f�X�g���N�^
		~AnimationSystem() = default;

		/// @brief ������
		void onCreate() override;
		/// @brief �폜��
		void onDestroy() override;
		/// @brief �X�V
		void onUpdate() override;

		/// @brief �Q�[���I�u�W�F�N�g�X�V�O�Ɉ�x�����Ă΂��R�[���o�b�N
		void onStartGameObject(const GameObjectID& id) override;

	private:

	};
}