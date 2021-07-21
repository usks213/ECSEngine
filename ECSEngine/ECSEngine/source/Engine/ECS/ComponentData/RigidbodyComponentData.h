/*****************************************************************//**
 * \file   RigidbodyComponentData.h
 * \brief  ���W�b�h�{�f�B
 * 
 * \author USAMI KOSHI
 * \date   2021/07/22
 *********************************************************************/
#pragma once

#include <Engine/ECS/Base/IComponentData.h>
#include <Engine/Utility/Mathf.h>


namespace ecs {

	/// @brief ���W�b�h�{�f�B
	struct Rigidbody : IComponentData
	{
		ECS_DECLARE_COMPONENT_DATA(Rigidbody);

		float mass;
		bool isCreate;

		/// @brief �R���X�g���N�^
		Rigidbody(float mass) :
			mass(mass), isCreate(false)
		{}
	};
}