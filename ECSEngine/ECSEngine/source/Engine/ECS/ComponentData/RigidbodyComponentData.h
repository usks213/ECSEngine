/*****************************************************************//**
 * \file   RigidbodyComponentData.h
 * \brief  リジッドボディ
 * 
 * \author USAMI KOSHI
 * \date   2021/07/22
 *********************************************************************/
#pragma once

#include <Engine/ECS/Base/IComponentData.h>
#include <Engine/Utility/Mathf.h>


namespace ecs {

	/// @brief リジッドボディ
	struct Rigidbody : IComponentData
	{
		ECS_DECLARE_COMPONENT_DATA(Rigidbody);

		float mass;
		bool isCreate;

		/// @brief コンストラクタ
		Rigidbody(float mass) :
			mass(mass), isCreate(false)
		{}
	};
}