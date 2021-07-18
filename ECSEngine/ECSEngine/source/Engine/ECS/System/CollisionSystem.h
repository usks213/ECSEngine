/*****************************************************************//**
 * \file   CollisionSystem.h
 * \brief  �����蔻��V�X�e��
 * 
 * \author USAMI KOSHI
 * \date   2021/07/18
 *********************************************************************/
#pragma once

#include <Engine/ECS/Base/SystemBase.h>

namespace ecs {

	class CollisionSystem final : public SystemBase
	{
		ECS_DECLARE_SYSTEM(CollisionSystem);
	public:
		/// @brief �R���X�g���N�^
		/// @param pWorld ���[���h
		explicit CollisionSystem(World* pWorld):
			SystemBase(pWorld)
		{}
		/// �f�X�g���N�^
		~CollisionSystem() = default;

		/// @brief ������
		void onCreate() override;
		/// @brief �폜��
		void onDestroy() override;
		/// @brief �X�V
		void onUpdate() override;

	private:
		void Collision(const Entity& main, const std::vector<Entity>& m_subList);
	};
}