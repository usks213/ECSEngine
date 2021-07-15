/*****************************************************************//**
 * \file   BasicComponentData.h
 * \brief  ��{�R���|�[�l���g�f�[�^
 * 
 * \author USAMI KOSHI
 * \date   2021/07/05
 *********************************************************************/
#pragma once

#include <Engine/ECS/Base/IComponentData.h>
#include <Engine/Utility/Mathf.h>


namespace ecs {

	/// @brief ���O�R���|�[�l���g
	struct Name : IComponentData
	{
		ECS_DECLARE_COMPONENT_DATA(Name);
		char value[MAX_COMPONENT_STRING];
	};

	/// @brief �^�O�R���|�[�l���g
	struct Tag : IComponentData
	{
		ECS_DECLARE_COMPONENT_DATA(Tag);
		char value[MAX_COMPONENT_STRING];
	};
}