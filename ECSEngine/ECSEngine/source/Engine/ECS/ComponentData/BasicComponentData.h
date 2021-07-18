/*****************************************************************//**
 * \file   BasicComponentData.h
 * \brief  基本コンポーネントデータ
 * 
 * \author USAMI KOSHI
 * \date   2021/07/05
 *********************************************************************/
#pragma once

#include <Engine/ECS/Base/IComponentData.h>
#include <Engine/Utility/Mathf.h>


namespace ecs {

	/// @brief 静的オブジェクト
	struct StaticType : IComponentData
	{
		ECS_DECLARE_COMPONENT_DATA(StaticType);
	};

	/// @brief 動的オブジェクト
	struct DynamicType : IComponentData
	{
		ECS_DECLARE_COMPONENT_DATA(DynamicType);
	};
}