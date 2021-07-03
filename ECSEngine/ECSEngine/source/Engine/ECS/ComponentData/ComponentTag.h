/*****************************************************************//**
 * \file   ComponentTag.h
 * \brief  コンポーネントタグ
 * 
 * \author USAMI KOSHI
 * \date   2021/07/02
 *********************************************************************/
#pragma once
#include <Engine/ECS/Base/IComponentData.h>


namespace ecs {

	struct PlayerTag : IComponentData
	{
		ECS_DECLARE_COMPONENT_DATA(PlayerTag);
	};

	struct InputTag : IComponentData
	{
		ECS_DECLARE_COMPONENT_DATA(PlayerTag);
	};

	struct DebugTag : IComponentData
	{
		ECS_DECLARE_COMPONENT_DATA(PlayerTag);
	};

}