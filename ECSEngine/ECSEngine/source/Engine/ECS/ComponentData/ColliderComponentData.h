/*****************************************************************//**
 * \file   ColliderComponentData.h
 * \brief�@�R���C�_�[
 * 
 * \author USAMI KOSHI
 * \date   2021/07/22
 *********************************************************************/
#pragma once

#include <Engine/ECS/Base/IComponentData.h>
#include <Engine/Utility/Mathf.h>


namespace ecs {

	/// @brief �����R���|�[�l���g�f�[�^
	struct Collider : IComponentData
	{
		ECS_DECLARE_COMPONENT_DATA(Collider);

		/// @brief �R���C�_�[�̎��
		enum class ColliderType : std::uint32_t
		{
			UNKNOWN = 0, // ����`
			BOX,         // �L���[�u
			SPHERE,      // �X�t�B�A
			CYLINDER,    // �V�����_�[
			CAPSULE,     // �J�v�Z��
			CONE,        // �R�[��
			PLANE,       // ����
			POLYGON,     // �|���S��
			COMPOUND     // �����V�F�C�v
		};
		
		/// @brief �R���C�_�[�^�C�v
		ColliderType colliderType;
		/// @brief �����o���g�p
		bool trigger;

		/// @brief ���S�I�t�Z�b�g
		Vector3 center;
		/// @brief �T�C�Y
		Vector3 size;

		/// @brief �R���X�g���N�^
		Collider(ColliderType type, Vector3 center = Vector3(), Vector3 size = Vector3(1,1,1)):
		colliderType(type), trigger(false), center(center), size(size)
		{}

	};
}