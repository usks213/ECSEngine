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

	struct WorldMatrix : IComponentData
	{
		ECS_DECLARE_COMPONENT_DATA(WorldMatrix);
		Matrix value;
	};

}