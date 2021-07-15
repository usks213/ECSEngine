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
		Matrix			localMatrix;
		Matrix			globalMatrix;
		Vector3			localScale;
		Vector3			globalScale;
		Transform(const GameObjectID& id) :
			id(id), localScale(1,1,1), globalScale(1,1,1)
		{
		}
	};

	struct Position : IComponentData
	{
		ECS_DECLARE_COMPONENT_DATA(Position);
		Vector3 value;
	};

	struct Rotation : IComponentData
	{
		ECS_DECLARE_COMPONENT_DATA(Rotation);
		Quaternion value;
	};

	struct Scale : IComponentData
	{
		ECS_DECLARE_COMPONENT_DATA(Scale);
		Vector3 value;
	};
}