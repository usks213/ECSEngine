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

	/// @brief 名前コンポーネント
	struct Name : IComponentData
	{
		ECS_DECLARE_COMPONENT_DATA(Name);
		char value[MAX_COMPONENT_STRING];
	};

	/// @brief タグコンポーネント
	struct Tag : IComponentData
	{
		ECS_DECLARE_COMPONENT_DATA(Tag);
		char value[MAX_COMPONENT_STRING];
	};
}