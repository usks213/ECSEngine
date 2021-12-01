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

		Matrix			localMatrix;
		Matrix			globalMatrix;
		//Matrix			parentMatrix;
		//Vector3			localScale;
		Vector3			globalScale;

		Transform(const GameObjectID& id) :
			id(id), translation(), rotation(), scale(1,1,1),
			globalScale(1,1,1)
			//localScale(1,1,1), parentScale(1,1,1)
		{
		}
		Transform(const GameObjectID& id, Vector3 translation, 
			Quaternion rotation = Quaternion(), Vector3 scale = Vector3(1,1,1)) :
			id(id), translation(translation), rotation(rotation), scale(scale),
			globalScale(scale)
			//localScale(1, 1, 1), parentScale(1, 1, 1)
		{
			localMatrix = Matrix::CreateScale(scale);
			localMatrix *= Matrix::CreateFromQuaternion(rotation);
			localMatrix *= Matrix::CreateTranslation(translation);
			globalMatrix = localMatrix;
		}
	};
}