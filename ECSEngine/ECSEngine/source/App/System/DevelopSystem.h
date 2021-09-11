/*****************************************************************//**
 * \file   DevelopSystem.h
 * \brief  �J���p�V�X�e��
 * 
 * \author USAMI KOSHI
 * \date   2021/09/01
 *********************************************************************/
#pragma once

#include <Engine/ECS/Base/SystemBase.h>
#include <Engine/ECS/Base/GameObjectManager.h>


namespace ecs {

	class DevelopSystem final : public SystemBase
	{
		ECS_DECLARE_SYSTEM(DevelopSystem);
	public:
		/// @brief �R���X�g���N�^
		/// @param pWorld ���[���h
		explicit DevelopSystem(World* pWorld):
			SystemBase(pWorld)
		{}
		/// �f�X�g���N�^
		~DevelopSystem() = default;

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