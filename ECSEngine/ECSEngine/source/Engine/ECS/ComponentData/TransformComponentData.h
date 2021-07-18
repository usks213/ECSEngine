/*****************************************************************//**
 * \file   TransformComponentData.h
 * \brief  トランスフォームコンポーネンデータ
 * 
 * \author USAMI KOSHI
 * \date   2021/06/27
 *********************************************************************/
#pragma once

#include <Engine/ECS/Base/GameObject.h>
#include <Engine/Utility/Mathf.h>


namespace ecs {

	/// @brief トランスフォームデータ(必須)
	struct Transform : IComponentData
	{
		ECS_DECLARE_COMPONENT_DATA(Transform);
		GameObjectID	id;

		Vector3			translation;
		Quaternion		rotation;
		Vector3			scale;

		Matrix			localToWorld;
		Matrix			localToParent;
		//Vector3			localScale;
		Vector3			parentScale;

		Transform(const GameObjectID& id) :
			id(id), translation(), rotation(), scale(1,1,1)
			//localScale(1,1,1), parentScale(1,1,1)
		{
		}
		Transform(const GameObjectID& id, Vector3 translation, 
			Quaternion rotation = Quaternion(), Vector3 scale = Vector3(1,1,1)) :
			id(id), translation(translation), rotation(rotation), scale(scale)
			//localScale(1, 1, 1), parentScale(1, 1, 1)
		{
		}
	};
}