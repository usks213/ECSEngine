/*****************************************************************//**
 * \file   TransformComponentData.h
 * \brief  トランスフォームコンポーネンデータ
 * 
 * \author USAMI KOSHI
 * \date   2021/06/27
 *********************************************************************/
#pragma once

#include <Engine/ECS/Base/IComponentData.h>
#include <Engine/Utility/Mathf.h>


namespace ecs {

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

	struct LocalToWorld : IComponentData
	{
		ECS_DECLARE_COMPONENT_DATA(LocalToWorld);
		Matrix value;
	};

	struct LocalToParent : IComponentData
	{
		ECS_DECLARE_COMPONENT_DATA(LocalToParent);
		//Matrix value;
		Entity parent;
		LocalToParent(Entity& entity) : 
			parent(entity)
		{}
	};
}