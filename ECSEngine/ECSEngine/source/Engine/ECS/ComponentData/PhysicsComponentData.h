/*****************************************************************//**
 * \file   PhysicsComponentData.h
 * \brief  �����R���|�[�l���g�f�[�^
 * 
 * \author USAMI KOSHI
 * \date   2021/07/18
 *********************************************************************/
#pragma once

#include <Engine/ECS/Base/IComponentData.h>
#include <Engine/Utility/Mathf.h>


namespace ecs {

	/// @brief �����R���|�[�l���g�f�[�^
	struct Physics : IComponentData
	{
		ECS_DECLARE_COMPONENT_DATA(Physics);

		/// @brief �R���C�_�[�̎��
		enum class ColliderType : std::uint32_t
		{
			Box,
			Sphere,
			Max,
		};

		/// @brief �R���X�g���N�^
		/// @param collider �R���C�_�[���
		/// @param trigger �����o���Ȃ�
		/// @param gravity �d�͎g�p
		explicit Physics(ColliderType collider, bool trigger = false, bool gravity = true) :
			colliderType(collider), trigger(trigger), 
			gravity(gravity), mass(1.0f), drag(),
			e(0.5f), staticFriction(0.1f), dynamicFriction(0.05f)
		{
		}

		//--- �����p�����[�^

		/// @brief �R���C�_�[�^�C�v
		ColliderType colliderType;
		/// @brief �����o���g�p
		bool trigger;

		/// @brief �d�͎g�p
		bool gravity;
		/// @brief ����
		float mass;
		/// @brief ��R
		Vector3 drag;
		/// @brief �����e���\��?
		// Quaternion m_

		/// @brief �����W��
		float e;
		/// @brief �Ö��C�W��
		float staticFriction;
		/// @brief �����C�W��
		float dynamicFriction;

		//--- ���������p�����[�^
		/// @brief ���x
		Vector3 velocity;
		/// @brief ��
		Vector3 force;
		/// @brief �p���x
		Quaternion angularVelocity;
		/// @brief ���[�����g
		Quaternion torque;
	};
}