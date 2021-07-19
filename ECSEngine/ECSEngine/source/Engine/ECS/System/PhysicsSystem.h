/*****************************************************************//**
 * \file   PhysicsSystem.h
 * \brief  �����V�X�e��
 * 
 * \author USAMI KOSHI
 * \date   2021/07/18
 *********************************************************************/
#pragma once

#include <Engine/ECS/Base/SystemBase.h>
#include <Engine/Utility/Mathf.h>


namespace ecs {

	class PhysicsSystem final : public SystemBase
	{
		ECS_DECLARE_SYSTEM(PhysicsSystem);
	public:
		/// @brief �R���X�g���N�^
		/// @param pWorld ���[���h
		explicit PhysicsSystem(World* pWorld):
			SystemBase(pWorld)
		{}
		/// �f�X�g���N�^
		~PhysicsSystem() = default;

		/// @brief ������
		void onCreate() override;
		/// @brief �폜��
		void onDestroy() override;
		/// @brief �X�V
		void onUpdate() override;

	private:
		/// @brief �d��
		Vector3 m_graviyAcceleration;
	};
}