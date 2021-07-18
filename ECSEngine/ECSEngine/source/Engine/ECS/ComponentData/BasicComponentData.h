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

	/// @brief �ÓI�I�u�W�F�N�g
	struct StaticType : IComponentData
	{
		ECS_DECLARE_COMPONENT_DATA(StaticType);
	};

	/// @brief ���I�I�u�W�F�N�g
	struct DynamicType : IComponentData
	{
		ECS_DECLARE_COMPONENT_DATA(DynamicType);
	};
}